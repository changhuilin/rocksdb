include ./make_config.mk

.PHONY: clean

all: simple_example column_families_example compact_files_example c_simple_example

CXXFLAGS += -std=c++11 -g -O2 -fPIC $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)
LIBS= -L$(ROCKSDB_LIB) -lrocksdb
INCLUDES= -I$(ROCKSDB_HOME)/include

simple_example: simple_example.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@  $(INCLUDES) $(LIBS)

column_families_example: column_families_example.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@  $(INCLUDES) $(LIBS)

compact_files_example: compact_files_example.cc
	$(CXX) $(CXXFLAGS) $@.cc -o$@  $(INCLUDES) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

c_simple_example: c_simple_example.o
	$(CXX) $@.o -o$@ $(INCLUDES) $(LIBS)

clean:
	rm -rf ./simple_example ./column_families_example ./compact_files_example ./c_simple_example c_simple_example.o
