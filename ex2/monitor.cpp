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
#include "fileHandler.h"
#define MSGSIZE  250




int main(int argc, char const *argv[]){

    if(argc!=5) {std::cout<<"wrong num of arguments given \n"; return -1;}

    char buf[MSGSIZE];
    int fd1,fd2;
    char mypipe1[25];
    char mypipe2[25];
    bool endFlag=false;
    strcpy(mypipe1,(argv[1]));
    strcpy(mypipe2,(argv[2]));
    int bufSize=atoi(argv[3]);
    char message[MSGSIZE];

    if((fd1 = open(mypipe1, O_RDONLY))<0){std::cout<<"pipe open error in reader pipe "<<mypipe1<<"with error "<<errno <<"\n";return -1;}
    if((fd2 = open(mypipe2, O_WRONLY ))<0){std::cout<<"pipe open error in writer pipe "<<mypipe1<<"with error "<<errno <<"\n";return -1;}

    std::string* temp;
    char type[4];
    List<std::string>* filesName=NULL;
    Hash<Record>* recs =NULL;
    Hash<Virus>* virs =NULL;
    while(!endFlag){
      if(myRead(bufSize,fd1,message)!=2){std::cout<<"expected to read char \n"; return -1;}
      if(strcmp(message,"no more")==0) endFlag=true;
      else filesName=fileGetter(filesName,message);
    }
    dataInsert(&recs,&virs , &filesName , atoi(argv[4]));
    bloomReturn(bufSize,fd2,int(atoi(argv[4])/4),&virs);

    int act;
    bool badFlag;
    endFlag=false;
    std::string id,virus,date;
    endFlag=false;
    while(!endFlag){
      if(myRead(bufSize,fd1,message)!=1){std::cout<<"expected int but read char \n"; return -1; }
      act=atoi(message);
      badFlag=true;
      bool res;
      Virus* v;
      Patient* p;

      switch(act) {

          case 1: // requests
                if(myRead(bufSize,fd1,message)!=2){std::cout<<"expected to read char \n"; return -1;}
                id=std::string(message);
                if(myRead(bufSize,fd1,message)!=2){std::cout<<"expected to read char \n"; return -1;}
                virus=std::string(message);
                if(myRead(bufSize,fd1,message)!=2){std::cout<<"expected to read char \n"; return -1;}
                date=std::string(message);
                v=virs->searchInHash(virus);
                //we know virus is in hash from main
                p= v->getVaccPatRecord(id);
                if(p==NULL)std::cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                else{
                  res=sixMonthsChecker(p->getDate(),date);
                  if(res)std::cout<<"REQUEST ACCEPTED – HAPPY TRAVELS\n";
                  else std::cout<<"REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE \n";
                }
                badFlag=false;
                break;
          case 3:
                endFlag=true;
                badFlag=false;
                break;
      }//switch
      if(badFlag) std::cout << "INCORRECT DATA GIVEN in monitor" << '\n';
    }//while




   close(fd1);
   close(fd2);


    delete virs;
    delete recs;
    delete filesName;



    return 0;
}
