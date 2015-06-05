#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h> /* mmap() is defined in this header */
#include<fcntl.h>

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>

#include<vector>
#include<queue>
#include<sstream>
#include<iostream>
#include<fstream>
#include<random>

#include "logging.h"
#include "cycles.h"

//#define DEBUG


struct mem_data_t {
  void *    ptr;
  size_t    total_len;
  void *    key;
  size_t    key_len;
  unsigned  idx;

  mem_data_t(void* _ptr, size_t _total_len, void* _key, size_t _key_len, unsigned _idx) {
    ptr = _ptr;
    total_len = _total_len;
    key = _key;
    key_len = _key_len;
    idx = _idx;
  }
};


/**
 * compare memory areas -- byte-wise compare
 */
class CompareMemory {
  public:
    bool operator() (mem_data_t& d1, mem_data_t& d2) {

      const size_t min_len = (d1.key_len < d2.key_len) ? d1.key_len : d2.key_len;
      int r = memcmp(d1.key, d2.key, min_len);

      if (r == 0) {
        if (d1.key_len < d2.key_len) r = -1;
        else if (d1.key_len > d2.key_len) r = +1;
      }

      if(r < 0 ) 
        return false;
      else 
        return true;
    }
};

// parse each key-value pair
void parse_kv(char* ptr, void** key, size_t* key_len, size_t* total_len) {

  *key_len = *((uint8_t*)ptr);
  *key = ptr+1;

  ptr += (1 + *key_len);

  uint8_t val_len = *((uint8_t*)ptr);

  *total_len = 2 + *key_len + val_len;
}


// main function
int main(int argc, char *argv[]) {

  if(argc < 2) {
    printf("[USAGE]: ./merge_sort <num_files> <file1> ... \n");
    printf("         ./merge_sort <num_files>\n");
    exit(1);
  }

  unsigned num_files = atoi(argv[1]);
  printf("argc = %d, num_files = %u\n", argc, num_files);

  if(!(argc == (int)(2 + num_files) || argc == 2)) {
    printf("[USAGE]: ./merge_sort <num_files> <file1> ... \n");
    printf("         ./merge_sort <num_files>\n");
    exit(1);
  }

  unsigned total_num_pairs = 0;

  int* fdin = (int*)calloc(num_files, sizeof(int));
  struct stat* statbuf = (struct stat*)calloc(num_files, sizeof(struct stat));
  void** ptr = (void**)calloc(num_files, sizeof(void*));
  char** p_read = (char **)calloc(num_files, sizeof(char *));
  unsigned* num_pairs = (unsigned *)calloc(num_files, sizeof(unsigned));
  unsigned* num_pairs_idx = (unsigned *)calloc(num_files, sizeof(unsigned));

  const cpu_time_t tsc_per_sec = (unsigned long long)get_tsc_frequency_in_mhz() * 1000000UL;
  PLOG("tsc_per_sec = %lu", tsc_per_sec);

  for(unsigned file_idx = 0; file_idx < num_files; file_idx++) {

    char file_name[1024];

    if(argc==2) {
      std::stringstream ss;
      ss << "table-" << file_idx << ".txt";
      strcpy(file_name, ss.str().c_str());
    } else {
      strcpy(file_name, argv[2+file_idx]);
    }

    if ((fdin[file_idx] = open(file_name, O_RDONLY)) < 0) {
      PERR("can't open %s for reading", file_name);
      assert(false);
    }

    /* find size of input file */
    if (fstat (fdin[file_idx], &statbuf[file_idx]) < 0) {
      PERR("fstat error");
      assert(false);
    }

    /* mmap the input file */
    if ((ptr[file_idx] = mmap (0, statbuf[file_idx].st_size, PROT_READ, MAP_SHARED, fdin[file_idx], 0)) == (caddr_t) -1) {
      PERR("mmap error for input");
      assert(false);
    }

    p_read[file_idx] = (char *)ptr[file_idx];

    num_pairs[file_idx] = *((unsigned *)(p_read[file_idx]));
    p_read[file_idx] += sizeof(unsigned);
    PINF("(file %u) %u key-value pairs in total", file_idx, num_pairs[file_idx]);

  }

  // open the output file
  std::ofstream* ofs = new std::ofstream("output.txt", std::ios::out | std::ios::binary);
  unsigned num_records_output = 0;
  ofs->write((char *)&num_records_output, sizeof(unsigned));

  cpu_time_t start_tsc = rdtsc();

  // min heap
  std::priority_queue<mem_data_t, std::vector<mem_data_t>, CompareMemory> pq;

  // put the first record of each table to the min heap
  for(unsigned file_idx = 0; file_idx < num_files; file_idx++) {
    void *key;
    size_t key_len, total_len;

    parse_kv(p_read[file_idx], &key, &key_len, &total_len);

#ifdef DEBUG
    std::string key_str(((char*)(p_read[file_idx])+1), key_len);
    std::cout << key_str << std::endl;
#endif

    struct mem_data_t dm(p_read[file_idx], total_len, key, key_len, file_idx);

    pq.push(dm);

    num_pairs_idx[file_idx]++;

    p_read[file_idx] += total_len;
  }


  // pull the smallest key from the heap, and push the next one in the same table
  while(!pq.empty()) {

    struct mem_data_t md = pq.top();
    unsigned file_idx = md.idx;
    void *ptr = md.ptr;
    size_t total_len = md.total_len;

#ifdef DEBUG
    printf("size = %lu, file_idx = %u\n", pq.size(), file_idx);
    std::string key_str((char*)md.key, md.key_len);
    std::cout << "popped key: " << key_str << std::endl;
#endif

    // write the the output file
    ofs->write((char *)ptr, total_len);
    total_num_pairs++;

    pq.pop();
      
    // fetch the next record, and push it to the heap
    if(num_pairs_idx[file_idx] < num_pairs[file_idx]) {

      void *key;
      size_t key_len, total_len;

      parse_kv(p_read[file_idx], &key, &key_len, &total_len);

      struct mem_data_t md(p_read[file_idx], total_len, key, key_len, file_idx);
      assert((char *)md.ptr + 1 == (char *)md.key);

      pq.push(md);

      p_read[file_idx] += total_len;
      num_pairs_idx[file_idx]++;
    }
  }

  printf("total_pairs = %u\n", total_num_pairs);

  //close output file
  ofs->seekp(0);
  ofs->write((char *)&total_num_pairs, sizeof(unsigned));
  ofs->close();
  delete ofs;

  //close input files
  for(unsigned file_idx = 0; file_idx < num_files; file_idx++) {
    if (close(fdin[file_idx]) < 0) {
      PERR("closing file fails");
      assert(false);
    }
  }

  //stats
  cpu_time_t end_tsc = rdtsc();

  double tsc_secs = (end_tsc - start_tsc)*1.0 / tsc_per_sec;

  printf("\n** (Merge Sort) %u key-value pairs in %2f sec\n", total_num_pairs, tsc_secs);



  return 0;
}
