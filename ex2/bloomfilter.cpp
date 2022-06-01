#include "bloomfilter.h"


BloomFilter::BloomFilter(unsigned int num){
    this->num_of_bytes=num;
    this->bytes= new unsigned int[num_of_bytes/4];
    for (int i=0; i<this->num_of_bytes/4; i++ ) this->bytes[i]=0;

}

BloomFilter::BloomFilter(unsigned  int num,unsigned int bytesAsNum ){

    this->num_of_bytes=num;
    this->bytes= new unsigned int[num_of_bytes/4];
    for (int i=0; i<this->num_of_bytes/4; i++ ) this->bytes[i]=0;

    int bitsInByte[U_SIZE];
    unsigned int n;
}

unsigned int BloomFilter::ReturnAsNum(){
    
    return 1;

}


void BloomFilter::printBloom(){
    std::cout<<"===========================\n";
    for(int i=0 ; i<this->num_of_bytes/4; i++){
        std::cout<<i<<". "<<this->bytes[i]<<"\n";
    }
    std::cout<<"===========================\n";

}

unsigned int BloomFilter::getNumInPos(int pos){
    return this->bytes[pos];
}

void BloomFilter::setNumInPos(int pos , unsigned int num){
    this->bytes[pos]=num;
}


BloomFilter::~BloomFilter(){
    delete[] this->bytes;
}


void BloomFilter::bitChange(unsigned int bitNum){
    int bytePos= int(bitNum/U_SIZE);
    int bitPos= bitNum%U_SIZE;
    int bitsInByte[U_SIZE];
    int j,temp;
    for( j=0; j<U_SIZE; j++) bitsInByte[j]=0;
    int i = 0;
    unsigned int n= this->bytes[bytePos];
    while (n > 0){
        bitsInByte[i] = n % 2;
        n = n / 2;
        i++;
    }
    bitsInByte[bitPos]=1;
    for(j=0; j<U_SIZE; j++){
         n+= int((pow(2.0,double(j)))*bitsInByte[j]);
    }
    this->bytes[bytePos]=n;

}

int BloomFilter::bitSearch(unsigned int bitNum){
    int bytePos= int(bitNum/U_SIZE);
    int bitPos= bitNum%U_SIZE;
    int bitsInByte[U_SIZE];
    int j,temp;
    for( j=0; j<U_SIZE; j++) bitsInByte[j]=0;
    int i = 0;
    unsigned int n= this->bytes[bytePos];
    while (n > 0){
        bitsInByte[i] = n % 2;
        n = n / 2;
        i++;
    }
    return bitsInByte[bitPos] ;
}


void BloomFilter::wordInsert(char* str){
    int i;
    for(i=0; i< K; i++){
        this->bitChange(hash_i((unsigned char*)str,i));
    }
}

int BloomFilter::wordSearch(char* str){
    int i,flag;
    for(i=0; i< K; i++){
        flag=this->bitSearch(hash_i((unsigned char*)str,i));
        if(flag==0) return 0;
    }
    return 1;
}


unsigned int BloomFilter::djb2(unsigned char* str) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash%(this->num_of_bytes);
    }

unsigned long BloomFilter::hash_i(unsigned char *str, unsigned int i) {
    return (djb2(str) + i*sdbm(str) + i*i)%(this->num_of_bytes*8);
}

unsigned long BloomFilter::sdbm(unsigned char *str) {
    unsigned long hash = 0;
    int c;

    while (c = *str++) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}