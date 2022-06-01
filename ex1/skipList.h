#ifndef __SKIP__
#define __SKIP__

#include <iostream>
#include <string>
#include <cstdlib>
#include "records.h"

#define MAX_LEVEL 33
#define P 50

class SLnode;

struct PlainList;

class skipList
{
    private:
        class SLnode** lvlHeader;


    public:
        skipList();
        ~skipList();
        SLnode* searchSL(int rId);
        void deleteSL(int rId);
        void insertSL(int rId,Record* r, std::string str);
        void PrintSlist();
        PlainList* asPlain();

};

class SLnode
{
    private:
        int id;
        Record* rec;
        std::string date;
        int level;
        class SLnode** next;
        class SLnode** prev;
    public:
        SLnode(int rId,Record* r  , std::string str, int lvl);
        ~SLnode();
        friend class skipList;
        std::string getDate(){
            return this->date;
        }
        //void setNext(int level , class SLnode* node);
        //class SLnode* getNode(int level ,char ans);

};

struct  PlainList{
        std::string date;
        Record* rec;
        PlainList* next;

};

int randomlvl() ;
void insertPlain(PlainList** first, Record* r , std::string d);
void deletePlain(PlainList* first);
void printPlain(PlainList* first);
PlainList* popFirst(PlainList** first);

#endif
