#ifndef __RECS__
#define __RECS__

#include <iostream>
#include <string>
#include "hashing.h"
#include <cstring>
#include <cstdio>
#include <ctime>
class Record
{
    private:
        int id;
        std::string name;
        std::string surname;
        std::string country;
        short int age;
        class Record* next;
    public:
        Record(std::string str_id,std::string name,std::string surname,std::string country,std::string str_age);
        ~Record();
        Record* addRec(Record* rec );
        Record* searchRec(int rId );
        void printRec();
        friend class RecordHash;
        int getId(){
            return this->id;
        }
        int getAge(){
            return this->age;
        }
        std::string getCountry(){
            return this->country;
        }

};
class RecordHash
{

    private:
        class Record** recsTable;
        int reCount;
        int hashSize;

    public:
        RecordHash(int hashSize);
        ~RecordHash();
        Record* addEntry(Record* rec);
        Record* searchEntry(int rId);
        void printHash();
};

bool dateChecker(std::string date1, std::string date2, std::string real);
bool dateValidor(std::string date);
std::string getTodaysDate();
int int_checker(std::string str_id);

#endif
