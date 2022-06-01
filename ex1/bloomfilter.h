#ifndef __BLOOM__
#define __BLOOM__

#include <iostream>
#include <string>
#include <cmath>
#include <cstring>
#include "hashing.h"
#define NUM_OF_BYTES 100000
#define U_SIZE 32 //unsigned int size
#define K 16



class BloomFilter
{
    private:
        unsigned int* bytes;
        unsigned int num_of_bytes;

    public:
        BloomFilter(unsigned  int num);
        ~BloomFilter();
        void bitChange(unsigned int bitNum);
        int bitSearch(unsigned int bitNum);
        void wordInsert(char* str);
        int wordSearch(char* str);
        
};

#endif
