#ifndef __COUNTRY_STATS__
#define __COUNTRY_STATS__

#include <iostream>
#include <string>
#include "bloomfilter.h"
#include "skipList.h"

#define NUM_OF_COUNTRIES 200


class CountryNode
{
    private:
        std::string name;
        double vaccinated;
        double general;
        double* agesYes;
        class CountryNode* next;


    public:
        CountryNode(std::string name , bool ages);
        void statsByCountry(std::string date1, std::string date2 , PlainList* vaccList , PlainList* nonVaccList,  bool ages);
        ~CountryNode();
        void insertCountry(std::string name );
        void PrintCNode();
        CountryNode* searchCountry(CountryNode* first, std::string name , bool ages);

};

#endif
