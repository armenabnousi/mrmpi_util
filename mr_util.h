#pragma once
#include "mpi.h"
#include "mapreduce.h"
#include "keyvalue.h"
#include <list>
#include <vector>
using namespace MAPREDUCE_NS;

#define SEPARATOR '!'

typedef struct CollectionContainer {
	int local_size;
	std::list< std::vector<char> > local_list;
	CollectionContainer(int initial_size) : local_size(initial_size) { }
} CollectionContainer;

class MrToolbox {

public:
	void static collect_keys(MapReduce* mr, std::vector<char>* collection);
	void static collect_local_keys(char* key, int keybytes, char* values, int nvalues, int* valuebytes, void* toolbox_ptr);
	void static prepare_array(CollectionContainer* container, char keys_array[]);
	void static remove_redundant(MapReduce* mr);
	void static redundant_agglomerator(char* key, int keybytes, char* values, int nvalues, int* valuebytes, KeyValue* kv, void* ptr);
	void static redundant_remover(char* key, int keybytes, char* values, int nvalues, int* valuebytes, KeyValue* kv, void* ptr);
	void static set_mr_params(MapReduce* mr, int pagesize);
};
