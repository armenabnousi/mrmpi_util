#Utility functions for MRMPI library</br>
**NOTE: The functions in this library are all marked as public since they are used in different parts in my programs ("legacy"), but for your personl use you can keep the main functions described below as public and change the rest to private.**</br>
</br>
This library provides functions that are frequently needed when working with MRMPI library (C++ implementation of MapReduce - http://mapreduce.sandia.gov/ ). I write these functions as I find the need for them. They can be updated and/or appended with new utilites anytime. Most of these functions are `static` and can be used without instantiation of an object of this class.</br>
</br>
**remove_redundant:**</br>
This functions accepts a pointer to the MapReduce object that contains set of `KV`s (not `KMV`s). It removes the repeated `<key, value>` pairs, retaining only one instance of each such pair. It performs by generating a `<key!value, NULL>` for each `<key, value>`, collating the `KV`s and then extracting again the remaining keys and values from the existing `KV`. Note that `!` is used as a separator between key and value and your original keys should not contain this character (or you can change the separator character from the header file).
```c++
//mr is an instance of MapReduce that contains KVs.
MrToolbox::remove_redundant(mr);
```
</br>
**collect_keys:**</br>
This function collect all the keys presented in the `KMV` and uses the MPI_Gatherv fuction to collect them all in a root processor. The collected keys will be inserted in a std::vector<char> that you will pass in a reference to it as an output argument.
```c++
//mr is an instance of MapReduce that contains KMVs.
std::vector<char> keys_vector;
MrToolbox::collect_keys(mr, &keys_vector);
```
</br>
**set_mr_params:**</br>
This function sets some of the parameters described in the `Settings` section of the MRMPI documentation. These parameters are memsize, ourofcore, verbosity and timer, as described in the documnetation.
```c++
int memsize = 1024;
int timer = 1;
int verbosity = 0;
int outofcore = -1;
MrToolbox::set_mr_params(mr, memsize, timer, verbosity, outofcore);
```
</br>
**gather_kmv:**</br>
This function simulates the `gather(int)` method from the MapReduce library which originally works only on KeyValue objects, for KeyMultiValue objects. It extracts KeyValue pairs from the given MapReduce object containing KeyMultiValues, and then uses the original `gather(int)` function using the provided number of processors (default 1) to collect all the key-values in the desired number of processors. The last parameter is a boolean (default true), which if true means the function will convert the collected key-values back into KeyMultiValue objects (using the `convert()` method of the MapReduce library.
```c++
mr -> reduce(some_function, NULL); //to generate KeyMultiValue objects
MrToolbox::gather_kmv(mr, 1, true);
//All keyMultiValues are gathered in processor 0
```
