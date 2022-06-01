#ifndef __FILHAN__
#define __FILHAN__

#include <iostream>
#include "hashTemplate.h"
#include <string>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>
#include<sys/socket.h>
#include "records.h"
#include "virusData.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <pthread.h>
#define MSGSIZE 250
#define RECSIZE 11003
#define VIRSIZE 17


struct fileGetArgs
{
    List<std::string>** filesName;
    List<std::string>** countries;
    Hash<Virus>** virs;
    Hash<Record>** recs;
    Node<int>** lastFilePos;
    int numThreads;
    int thrBufSize;
    int bloomSize;
    int sock;
    int sockBufSize;
};





void* mamaThreadFileGetter ( void * args);
void *kidThreadFileGetter(void * args);

void  *mamaAddVaccinations ( void * args);

std::string stringListToPaths(List<std::string>* l);
void print(std::string str);
void print(unsigned int num);

List<std::string>*  stringPathsToList(std::string paths);

int fileGetter(List<std::string>** l, char* dirName);
char* directorize( char* name);

void dataInsert(Hash<Record>** recs, Hash<Virus>** virs ,  int bloomSize, std::string fn);

int myRead(int bufSize, int fdRead, char* message );

int myWrite(int bufSize,int fdWrite,void* message,char* messType );


std::string unDirectorize(std::string mama, std:: string dir);
std::string unDirectorize2( std::string  dir);
bool dateChecker(std::string date1, std::string date2, std::string real);
bool dateValidor(std::string date);
std::string getTodaysDate();
int int_checker(std::string str_id);
bool sixMonthsChecker(std::string past , std::string future);
void bloomReturn(int bufSize , int fdWrite ,int bloomSize, Hash<Virus>** virs );

std::string generateSixMonthsBack(std::string date);
std::string generateSixMonthsForward(std::string date);
void bloomGet(int bufSize , int fdRead,int bloomSize, processData* pD);

#endif