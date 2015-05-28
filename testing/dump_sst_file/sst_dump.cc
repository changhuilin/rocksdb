// Copyright (c) 2013, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.
#include <cstdio>
#include <string>

#include "util/sst_dump_tool_imp.h"
#include "table/block_based_table_reader.h"

using namespace rocksdb;


int main(int argc, char** argv) {
    if(argc != 3) {
        printf("argc = %d\n", argc);
        printf("./sst_dump <sst_file_name> <dump_file_name>\n");
        exit(1);
    }

    SstFileReader sst_reader(argv[1], true, true);
    sst_reader.DumpTable(argv[2]);

    return 0; 
}
