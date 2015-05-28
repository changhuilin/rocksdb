include ../make_config.mk.test

.PHONY: clean

all: sst_dump

GTEST_DIR = $(ROCKSDB_HOME)/third-party/gtest-1.7.0/fused-src
GTEST = $(GTEST_DIR)/gtest/gtest-all.o
TESTUTIL = $(ROCKSDB_HOME)/util/testutil.o
TESTHARNESS = $(ROCKSDB_HOME)/util/testharness.o $(TESTUTIL) $(GTEST)

CXXFLAGS += -std=c++11 -g -O2 -fPIC $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)
LIBS= -L$(ROCKSDB_LIB) -lrocksdb
INCLUDES= -I$(ROCKSDB_HOME)/include -I$(ROCKSDB_HOME) -I$(GTEST_DIR)

sst_dump: sst_dump.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@  $(INCLUDES) $(LIBS)

clean:
	rm -rf ./sst_dump
