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
#include <filesystem>
#include "records.h"
#include "virusData.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#define MSGSIZE 250
#define RECSIZE 11003
#define VIRSIZE 17

List<std::string>* fileGetter(List<std::string>* l, char* dirName);
char* directorize( char* name);

void dataInsert(Hash<Record>** recs, Hash<Virus>** virs , List<std::string>** filesName, int bloomSize);
int myRead(int bufSize, int fdRead, char* message );

int myWrite(int bufSize,int fdWrite,void* message,char* messType );

//int preRead(int bufSize,int* mesSize,  char* reader,char* writer);

//int preWrite(int bufSize,void* message,char* messType ,  char* writer,char* reader);
std::string unDirectorize(std::string mama, std:: string dir);
bool dateChecker(std::string date1, std::string date2, std::string real);
bool dateValidor(std::string date);
std::string getTodaysDate();
int int_checker(std::string str_id);
bool sixMonthsChecker(std::string past , std::string future);
void bloomReturn(int bufSize , int fdWrite ,int bloomSize, Hash<Virus>** virs );

std::string generateSixMonthsBack(std::string date);
void bloomGet(int bufSize , int fdRead,int bloomSize, processData* pD);

#endif