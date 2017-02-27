#Utility functions for MRMPI library</br>
</br>
This library provides functions that are frequently needed when working with MRMPI library (C++ implementation of MapReduce - http://mapreduce.sandia.gov/ ). I write these functions as I find the need for them. They can be updated and/or appended with new utilites anytime.</br>
</br>
**remove_redundant:**</br>
This functions accepts a pointer to the MapReduce object that contains set of `KV`s (not `KMV`s). It removes the repeated `<key, value>` pairs, retaining only one instance of each such pair. It performs by generating a `<key!value, NULL>` for each `<key, value>`, collating the `KV`s and then extracting again the remaining keys and values from the existing `KV`. Note that `!` is used as a separator between key and value and your original keys should not contain this character (or you can change the separator character from the header file).
```c++
//mr is an instance of MapReduce that contains KVs.
remove_redundant(mr);
```
</br>
**collect_keys:**</br>
This function collect all the keys presented in the `KMV` and uses the MPI_Gatherv fuction to collect them all in a root processor. The collected keys will be inserted in a std::vector<char> that you will pass in a reference to it as an output argument.
```c++
//mr is an instance of MapReduce that contains KMVs.
std::vector<char> keys_vector;
collect_keys(mr, &keys_vector);
```
</br>
**set_mr_params:**</br>
This function sets some of the parameters described in the `Settings` section of the MRMPI documentation. These parameters are memsize, ourofcore, verbosity and timer, as described in the documnetation.
```c++
int memsize = 1024;
int timer = 1;
int verbosity = 0;
int outofcore = -1;
set_mr_params(mr, memsize, timer, verbosity, outofcore);
```