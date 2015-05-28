include ../make_config.mk.test

.PHONY: clean

all: gen_kv 

CXXFLAGS += -std=c++11 -g -O2 -fPIC $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)
LIBS= -L$(ROCKSDB_LIB) -lrocksdb
INCLUDES= -I$(ROCKSDB_HOME)/include -I$(ROCKSDB_HOME)

gen_kv: gen_kv.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@  $(INCLUDES) $(LIBS)

clean:
	rm -rf ./gen_kv
