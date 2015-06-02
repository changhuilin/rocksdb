#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h> /* mmap() is defined in this header */
#include<fcntl.h>

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

#include<vector>
#include<sstream>
#include<iostream>
#include<fstream>
#include<random>

#include "logging.h"


#define NUM_PAIRS (1024*8)
#define DEBUG_VERBOSE

/**
 *  Number of key-value pairs
 *  (unsigned)
 *
 *  For each pair
 *    1. (8-byte) Length of key
 *    2. Key
 *    3. (8-byte) Length of value
 *    4  Value
 *
 */

/**
 * Generate key-value pairs and write them to tables
 */
void write_tables(unsigned num_files) {

  std::vector<std::string> tables;
  std::vector<std::ofstream*> ofss;
  std::vector<unsigned> num_pairs;

  for(unsigned idx = 0; idx < num_files; idx++) {
    std::stringstream ss;

    ss << "table-" << idx << ".txt";
    tables.push_back(ss.str());

    //ofs.open("test.txt", std::ofstream::out | std::ofstream::app);
    std::ofstream* ofs = new std::ofstream(tables[idx].c_str(), std::ios::out | std::ios::binary);
    unsigned num_records = 0;
    ofs->write((char *)&num_records, sizeof(unsigned));
    ofss.push_back(ofs);

    num_pairs.push_back(0);
  }

  //Write to files
  unsigned base = 10000000;

  std::default_random_engine generator;
  std::uniform_int_distribution<unsigned> distribution(0,num_files-1);

  unsigned total_len = 0;

  for(unsigned idx = 0; idx < NUM_PAIRS; idx++) {

    char buffer[1024];
    char *p = buffer;

    uint8_t key_len, val_len, kv_len = 0;
    unsigned index = base + idx;

    //construct key
    std::stringstream ss_key;
    ss_key << "key-" << index;
    key_len = ss_key.str().size();

    memcpy(p, &key_len, sizeof(uint8_t));
    memcpy(p+1, ss_key.str().c_str(), key_len);

    p += (1+key_len);
    kv_len += (1+key_len);

    //construct value
    std::stringstream ss_val;
    ss_val << "val-" << index;
    val_len = ss_val.str().size();

    memcpy(p, &val_len, sizeof(uint8_t));
    memcpy(p+1, ss_val.str().c_str(), val_len);

    p += (1+val_len);
    kv_len += (1+val_len);

    unsigned target = distribution(generator);
    assert(target >=0 && target < num_files);

    ofss[target]->write(buffer, kv_len);

    num_pairs[target] = num_pairs[target] + 1;

    total_len += kv_len;
  }

  for(unsigned idx = 0; idx < num_files; idx++) { 
    ofss[idx]->seekp(0);
    ofss[idx]->write((char *)&num_pairs[idx], sizeof(unsigned));
    ofss[idx]->close();
    delete ofss[idx];
  }

  std::cout << "total_len = " << total_len << std::endl;
}


/**
 * Read a file using mmap
 */
void mmap_read(const char *input_file) {

  int fdin;
  void* ptr;
  struct stat statbuf;

  /* open the target file */
  if ((fdin = open (input_file, O_RDONLY)) < 0) {
    PERR("can't open %s for reading", input_file);
    assert(false);
  }

  /* find size of input file */
  if (fstat (fdin,&statbuf) < 0) {
    PERR("fstat error");
    assert(false);
  }

  /* mmap the input file */
  if ((ptr = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0))
      == (caddr_t) -1) {
    PERR("mmap error for input");
    assert(false);
  }

  char *p_read = (char*)ptr;

  unsigned num_pairs = *((unsigned *)p_read);
  p_read += sizeof(unsigned);
  PINF("%u key-value pairs in total", num_pairs);

  for(unsigned idx = 0; idx < num_pairs; idx++) {

    uint8_t len;
    char key[1024], val[1024];

    len = *((uint8_t*)p_read);

    memcpy(key, p_read+1, len);
    key[len] = '\0';
#ifdef DEBUG_VERBOSE
    printf("key = %s (len = %u) : ", key, len);
#endif

    p_read += (1+len);

    len = *((uint8_t*)p_read);

    memcpy(val, p_read+1, len);
    val[len] = '\0';
#ifdef DEBUG_VERBOSE
    printf("val = %s (len = %u)\n", val, len);
#endif

    p_read += (1+len);
  }
}

/**
 * Read a file using ifstream 
 */
void ifstream_read(const char *input_file, unsigned total_len) {

  std::ifstream ifs;
  ifs.open(input_file, std::ios::in | std::ios::binary);

  char *read_buffer = new char[total_len+1];
  char *p_read = read_buffer;

  ifs.read(read_buffer, total_len);

  unsigned num_pairs = *((unsigned *)p_read);
  p_read += sizeof(unsigned);
  PINF("%u key-value pairs in total", num_pairs);

  for(unsigned idx = 0; idx < num_pairs; idx++) {

    uint8_t len;
    char key[1024], val[1024];

    len = *((uint8_t*)p_read);

    memcpy(key, p_read+1, len);
    key[len] = '\0';
#ifdef DEBUG_VERBOSE
    printf("key = %s (len = %u) : ", key, len);
#endif

    p_read += (1+len);

    len = *((uint8_t*)p_read);

    memcpy(val, p_read+1, len);
    val[len] = '\0';
#ifdef DEBUG_VERBOSE
    printf("val = %s (len = %u)\n", val, len);
#endif

    p_read += (1+len);
  }

  delete[] read_buffer;

  ifs.close();
}


/**
 * compare memory areas -- byte-wise compare
 */
int compare(void *a_str, size_t a_sz, void *b_str, size_t b_sz) {

  const size_t min_len = (a_sz < b_sz) ? a_sz : b_sz;

  int r = memcmp(a_str, b_str, min_len);

  if (r == 0) {
    if (a_sz < b_sz) r = -1;
    else if (a_sz > b_sz) r = +1;
  }
  return r;
}

int compare(std::string a_str, std::string b_str) {

  size_t a_sz = a_str.size();
  size_t b_sz = b_str.size();

  return compare((void*)(a_str.c_str()), a_sz, (void*)(b_str.c_str()), b_sz);
}



//////////////////
// Main function
//////////////////
int main(int argc, char *argv[]) {

  if(argc != 3) {
    PERR("\nUSAGE: ./gen_files <w> <num_files> \n       ./gen_files <r> <file_name>");
    exit(1);
  } 
  
  if(strcmp(argv[1], "w")==0) { /* Write */

    unsigned num_files = atoi(argv[2]);
    PINF("Generate key-value pairs in %u files", num_files);

    write_tables(num_files);

  } else if(strcmp(argv[1], "r")==0) { /* Read */

//#define IFSTREAM_READ 1
#define MMAP_READ 1
    char input_file[32];
    strcpy(input_file, argv[2]);
#if (IFSTREAM_READ==1) 
    ifstream_read(input_file, 1024);
#elif (MMAP_READ==1)
    mmap_read(input_file);
#else
    PINF("NO READ !!!");
#endif

  } else {
    PERR("NOT Recognized!!");
  }

  return 0;
}


