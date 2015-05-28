// Copyright (c) 2013, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.
#include <cstdio>
#include <string>

#include "util/sst_dump_tool_imp.h"

using namespace rocksdb;


int main() {
    SstFileReader sst_reader("sst.txt", true, true);
    sst_reader.DumpTable("dump.txt");

    return 0; 
}
