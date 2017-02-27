#include "mr_util.h"

void MrToolbox::collect_keys(MapReduce* mr, std::vector<char>* collected_keys) {
	int me, nprocs;
	MPI_Comm_rank(MPI_COMM_WORLD, &me);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	CollectionContainer local_container(0);
	//if (me == 0) std::cout << "collecting keys: me: " << me << std::endl;
	mr -> scan(collect_local_keys, &local_container);
	//if (me == 0) std::cout << "local size is: " << local_container.local_size << std::endl;
	char local_array[local_container.local_size];
	prepare_array(&local_container, local_array);
	//if (me == 0) std::cout << "array prepared to be sent" << std::endl;
	int all_sizes[nprocs];
	MPI_Gather(&(local_container.local_size), 1, MPI_INT, all_sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//if (me == 0) std::cout << "local sizes received for gatherv" << std::endl;
	int total_size = 0;
	char* collected_keys_array = NULL;
	int displacements[nprocs];
	if (me == 0) {
		for (int i = 0; i < nprocs; i++) {
			displacements[i] = total_size;
			total_size += all_sizes[i];	
		}
	}
	collected_keys_array = new char[total_size];
	//if (me == 0) std::cout << "sending all keys " << std::endl;
	MPI_Gatherv(local_array, local_container.local_size, MPI_CHAR, collected_keys_array, all_sizes, 
			displacements, MPI_CHAR, 0, MPI_COMM_WORLD);
	//if (me == 0) std::cout << "all keys received; inserting in vector" << std::endl;
	collected_keys -> insert(collected_keys -> begin(), collected_keys_array, collected_keys_array + total_size);
	if(collected_keys_array) delete[] collected_keys_array;
	collected_keys_array = NULL;
}

void MrToolbox::collect_local_keys(char* key, int keybytes, char* values, int nvalues, int* valuebytes, void* toolbox_ptr) {
	CollectionContainer* toolbox = (CollectionContainer*) toolbox_ptr;
	std::vector<char> key_vector(key, key + keybytes);
	toolbox -> local_size += keybytes;
	(toolbox -> local_list).push_back(key_vector);
}

void MrToolbox::prepare_array(CollectionContainer* container, char keys_array[]) {
	int offset = 0;
	//std::cout << "local container size is: " << (container -> local_list).size() << std::endl;
	int me;
	MPI_Comm_rank(MPI_COMM_WORLD, &me);
	//if (me == 0) std::cout << "proc0 num keys is: " << (container -> local_list).size() << std::endl;
	while (!((container -> local_list).empty())) {
		std::vector<char> current = (container -> local_list).back();
		//if (me == 0) {
		//	std::cout << "current key in proc 0 is: ";
		//	for (int i = 0; i < current.size(); i++) {
		//		std::cout << current[i];
		//	}
		//	std::cout << std::endl;
		//}
		std::copy(current.begin(), current.end(), keys_array + offset);
		//std::cout << "current copied as: " << keys_array + offset;
		offset += current.size();
		(container -> local_list).pop_back();
	}
	//if (me == 0) std::cout << "all copied " << std::endl;
}

void MrToolbox::remove_redundant(MapReduce* mr) {
	mr -> reduce(redundant_agglomerator, NULL);
	mr -> convert();
	mr -> reduce(redundant_remover, NULL);
	mr -> convert();
}

void MrToolbox::redundant_agglomerator(char* key, int keybytes, char* values, int nvalues, int* valuebytes, KeyValue* kv, void* ptr) {
        int val_offset = 0;
        for (int val_index = 0; val_index < nvalues; val_index++)
        {
                char combinedkv[keybytes + 1 + valuebytes[val_index]];
                memcpy(combinedkv, key, keybytes);
                combinedkv[keybytes] = SEPARATOR;
                memcpy(combinedkv + keybytes + 1, values + val_offset, valuebytes[val_index]);
                kv -> add(combinedkv, keybytes + 1 + valuebytes[val_index], NULL, 0);
                val_offset += (valuebytes[val_index]);
        }
}

void MrToolbox::redundant_remover(char* key, int keybytes, char* values, int nvalues, int* valuebytes, KeyValue* kv, void* ptr) {
        int separator_pos = -1;
        for (int i = 0; i < keybytes; i++)
        {
                if ( key[i] == SEPARATOR)
                {
                        separator_pos = i;
                        break;
                }
        }
        char splitted_key[separator_pos];
        char splitted_val[keybytes - separator_pos - 1];
        memcpy(splitted_key, key, separator_pos);
        memcpy(splitted_val, key + separator_pos + 1, keybytes - separator_pos - 1);
        kv -> add(splitted_key, separator_pos, splitted_val, keybytes - separator_pos - 1);
}

void MrToolbox::set_mr_params(MapReduce* mr, int pagesize, int timer, int verbosity, int outofcore) {
	mr -> memsize = pagesize;
	mr -> verbosity = verbosity;
	mr -> timer = timer;
	mr -> outofcore = outofcore;
}

void MrToolbox::set_mr_params(MapReduce* mr, int pagesize) {
	//if (pagesize > 1024) pagesize = 1024;
	mr -> verbosity = 0;
	mr -> memsize = pagesize;
	mr -> timer = 0;
	mr -> outofcore = -1;
}


