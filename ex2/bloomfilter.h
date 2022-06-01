#ifndef __BLOOM__
#define __BLOOM__

#include <iostream>
#include <string>
#include <cmath>
#include <cstring>
#define U_SIZE 32 //unsigned int size
#define K 2



class BloomFilter
{
    private:
        unsigned int* bytes;
        unsigned int num_of_bytes;

    public:
        BloomFilter(unsigned  int num);
        BloomFilter(unsigned  int num,unsigned int bytesAsNum );
        ~BloomFilter();
        void printBloom();
        unsigned int ReturnAsNum();
        void bitChange(unsigned int bitNum);
        int bitSearch(unsigned int bitNum);
        void wordInsert(char* str);
        int wordSearch(char* str);      
        unsigned int getNumInPos(int pos);
        void setNumInPos(int pos , unsigned int num);


        unsigned int djb2(unsigned char* str);
        unsigned long hash_i(unsigned char *str, unsigned int i);
        unsigned long sdbm(unsigned char *str);

}; 

#endif
