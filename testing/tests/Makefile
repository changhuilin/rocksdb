include ../make_config.mk.test

.PHONY: clean

all: compaction_picker_test

CXXFLAGS += -std=c++11 -g -O2 -fPIC $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

INCLUDES= -I$(ROCKSDB_HOME)/include -I$(ROCKSDB_HOME) -I$(ROCKSDB_HOME)/third-party/gtest-1.7.0/fused-src

LIBS= -L$(ROCKSDB_LIB) -lrocksdb

GTEST_DIR = $(ROCKSDB_HOME)/third-party/gtest-1.7.0/fused-src
GTEST = $(GTEST_DIR)/gtest/gtest-all.o
TESTUTIL = $(ROCKSDB_HOME)/util/testutil.o
TESTHARNESS = $(ROCKSDB_HOME)/util/testharness.o $(TESTUTIL) $(GTEST)

compaction_picker_test: compaction_picker_test.cc
	$(CXX) $(CXXFLAGS) $@.cc $(TESTHARNESS) -o$@  $(INCLUDES) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	rm -rf ./compaction_picker_test
