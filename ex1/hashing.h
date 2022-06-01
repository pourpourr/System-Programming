#ifndef __HASHING__
#define __HASHING__
#include <iostream>
#include "bloomfilter.h"


unsigned long hash_i(unsigned char *str, unsigned int i) ;
unsigned long djb2(unsigned char *str);
unsigned long sdbm(unsigned char *str);

#endif
