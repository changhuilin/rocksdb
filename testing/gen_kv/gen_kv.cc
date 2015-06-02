// Copyright (c) 2013, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using namespace rocksdb;

std::string kDBPath = "./rocksdb_simple_example";

int main() {
  DB* db;
  Options options;
  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  //options.IncreaseParallelism();
  //options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;

  // open DB
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  // Put key-value
  s = db->Put(WriteOptions(), "keykeykey1", "value");
  assert(s.ok());
  std::string value;
  // get value
  s = db->Get(ReadOptions(), "keykeykey1", &value);
  assert(s.ok());
  assert(value == "value");

#define NUM_KEYS 500000
  for(unsigned idx = 0; idx < NUM_KEYS; idx++)
  {
      std::stringstream ss_key, ss_val;
      ss_key << "key-tt-" << idx;
      ss_val << "val-tt-" << idx;

      //std::cout << ss_key.str() << " : " << ss_val.str() << std::endl;
      s = db->Put(WriteOptions(), ss_key.str(), ss_val.str());
      assert(s.ok());
  }

  delete db;

  return 0;
}
