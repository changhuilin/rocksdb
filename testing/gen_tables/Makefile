
CXX_FLAGS=-DCONFIG_BUILD_DEBUG 

all: gen_tables merge_sort

gen_tables:
	g++ --std=gnu++11 -g -Wall $(CXX_FLAGS) gen_tables.cc -o gen_tables.o

merge_sort:
	g++ --std=gnu++11 -g -Wall $(CXX_FLAGS) merge_sort.cc -o merge_sort.o

clean:
	rm -rf *.o *.txt
