#ifndef __VIRUS_DATA__
#define __VIRUS_DATA__

#include <iostream>
#include <string>
#include "hashing.h"
#include "skipList.h"
#include "bloomfilter.h"

#define NUM_OF_VIRUSES 20

class VirusNode;

class VirusHash
{
    private:
        class VirusNode* viruses[NUM_OF_VIRUSES];

    public:
        VirusHash(unsigned int bloom);
        ~VirusHash();
        void PrintVHash();
        void insertVirusInHash(std::string vir ,Record* rec , std::string date , bool manual);
        void vaccineStatusBloom(std::string id, std::string vir);
        void vaccineStatus(int id ,std::string vir);
        void vaccineStatusAll(int id );
        PlainList* getLists(std::string vir, bool vacc);

};

class VirusNode
{
    private:
        std::string virus;
        BloomFilter* vaccBloom;
        skipList* vaccinated;
        skipList* nonVaccinated;
        unsigned int bloomsize;
        class VirusNode* next;


    public:
        VirusNode(int bloom);
        ~VirusNode();
        void insertVirus(std::string vir , Record* rec , std::string date );
        void insertVirusManual(std::string vir , Record* rec , std::string date );

        void PrintVNode();
        friend class VirusHash;

};


#endif
