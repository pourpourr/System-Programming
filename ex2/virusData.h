#ifndef __VIRUS_DATA__
#define __VIRUS_DATA__

#include <iostream>
#include <string>
#include "bloomfilter.h"
#include "hashTemplate.h"
#include "records.h"
#define PAT_SIZE 1013
static int pats=0;
static int virssss=0;

class Patient{
    private:
        Record* rec;
        std::string date;
    public:
        std::string getDate(){
            return this->date;
        }
        Patient(Record* r , std::string d);
        ~Patient();
        std::string strId(){
            return std::to_string(rec->getId());
        }
};

class Virus 
{
private:
    std::string virus;
    Hash<Patient>* vaccinated;
    Hash<Patient>* nonVaccinated;
    BloomFilter* vaccBloom;
public:
    Virus(std::string vir, unsigned int bloomSize);
    ~Virus();
    void insertPatient(Patient* pat);
    bool searchPatient(std::string id);
    Patient* getVaccPatRecord(std::string id);

    std::string getVir(){
        return this->virus;
    }
    BloomFilter** getBloom(){
        return &(this->vaccBloom);
    }
};

class processData{

    private:

        List<std::string>* countries;
        Node<BloomFilter>* virBlooms; 

    public:

        processData();
        ~processData();
        void insertCountry(std::string count);
        void insertBloom(std::string virus, BloomFilter* b);
       void printBlooms();
       bool isCountryInside(std::string country);
       int bloomStatus(std::string virus, std::string id);
};

#endif
