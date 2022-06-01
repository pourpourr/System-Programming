#include <iostream>
#include "hashTemplate.h"
#include <string>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>
#include <filesystem>
#include <dirent.h>
#include "fileHandler.h"
#include "records.h"
#define MSGSIZE  250 // max mess size


int main(int argc, char const *argv[])
{

    if(argc!=9) {std::cout<<"wrong num of arguments given \n"; return -1;}
    if(strcmp(argv[1],"-m")!=0 || strcmp(argv[3],"-b")!=0|| strcmp(argv[5],"-s")|| strcmp(argv[7],"-i")!=0){
        std::cout << "wrong imput give ./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir" << '\n';
        return -6;

    }

    DIR *d;
    if((d = opendir(argv[8]))==NULL){std::cout<<"bad dir name given\n"; return -5;}
    closedir(d);
    unsigned int bloom=int_checker(std::string(argv[6]));
    int numOfProc=atoi(argv[2]);
    int bufSize=atoi(argv[4]);
    if(bloom<=0 || numOfProc<=0 || bufSize<=3){
        std::cout << "invalid bloomsize(>0) or bufferSize(>=4) or NumOfMonitors(>0) given" << '\n';
        return -3;
    }
    ////////////////////////////////
    int bloomSize=int(atoi(argv[6])/4);

    pid_t* pids=new pid_t[numOfProc];
    int i;
    std::string tempStr;
    for(i=0; i<numOfProc; i++){
        if(mkfifo((std::to_string(i)+"in" ).c_str(), O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC |0777)==-1 &&errno!=EEXIST){ std::cout<<"mkfifo error "<<errno <<"\n" ;}
        if(mkfifo((std::to_string(i)+"out" ).c_str(), O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC | 0777)==-1 && errno!=EEXIST){ std::cout<<"mkfifo error "<<errno <<"\n" ;}
    }


    int n = numOfProc;
    int* fd1 = new int[numOfProc];
    int* fd2= new int[numOfProc];
    char mypipe1[25];
    char mypipe2[25];
    char message[MSGSIZE];
    char type[4];
    int writeRes,mesSize;
   for (i = 0; i < n; i++) {
        pids[i]=fork();
        switch(pids[i]){
                case-1: perror("fork  call"); exit (2);
                case 0: {
                   char* args[]={(char *)"./monitor",(char*)(std::to_string(i)+"out" ).c_str(),(char*)(std::to_string(i)+"in").c_str(),(char *)argv[4],(char*) argv[6], NULL };
                    execvp(args[0],args);
                    break;
                }
        }
    }



    for(i =0; i<n ; i++){
        strcpy(mypipe1,(std::to_string(i)+"out" ).c_str());
        strcpy(mypipe2,(std::to_string(i)+"in" ).c_str());
        if((fd1[i] = open(mypipe1, O_WRONLY))<0){std::cout<<"pipe open error in reader pipe "<<mypipe1<<"with error "<<errno <<"\n";return -1;}
        if((fd2[i] = open(mypipe2, O_RDONLY))<0){std::cout<<"pipe open error in reader pipe "<<mypipe2<<"with error "<<errno <<"\n";return -1;}
    }



    List<std::string>* filesName=NULL;
    std::string dir,diri2;
    diri2=std::string(argv[8]);
    filesName=fileGetter(filesName,(char *)diri2.c_str());
    std::string country;
    processData*  proc = new processData[numOfProc];

    i=0;
    strcpy(type,"cha");
    while(!filesName->isEmpty()){


        dir=filesName->popFirst();
        country= unDirectorize(diri2,dir);
        proc[i].insertCountry(country); //countries of the process
        myWrite(bufSize,fd1[i],(char*)dir.c_str(),type);
        if(++i==n)i=0;

    }
    strcpy(message,"no more");
    for(i=0; i<n; i++){
        myWrite(bufSize,fd1[i],message,type);
    }

    for(i=0; i<n; i++){
        bloomGet(bufSize,fd2[i],bloom,&proc[i]);
    }

    bool exitFlag=false,badFlag;
    std::string line,word,id,date,countryFrom,countryTo,virus,date1,date2;
    std::istringstream* iss;
    int act,monitorFrom,monitorTo,bloomRes;
    while(!exitFlag){

        std::cout << "give stuff" << '\n';
        if (!std::getline(std::cin, line) || line.empty()) badFlag=true;
        iss= new std::istringstream(line);
        (*iss)>>word;
        if(word=="/travelRequest") act=1;
        else if(word=="/searchVaccinationStatus")  act=2;
        else if(word=="/exit")  act=3;
        else act=4;

        badFlag=true;
        switch(act) {
            case 1:
            ///travelRequest citizenID date countryFrom countryTo virusName
                if(!(*iss>>id)) break;
                if(!(*iss>>date)) break;
                if(!(*iss>>countryFrom)) break;
                if(!(*iss>>countryTo)) break;
                if(!(*iss>>virus)) break;
                if((*iss>>word))break;
                monitorFrom=-1;
                monitorTo=-1;
                for(int i=0; i<numOfProc; i++){
                    if(proc[i].isCountryInside(countryFrom)){ monitorFrom=i; break;}
                }

                if(monitorFrom==-1 ){std::cout<<"COUNTRY_FROM NOT IN DATABASE \n"; break;}
                if(!dateValidor(date)) break;
                if(int_checker(id)<0){std::cout<<"BAD ID GIVEN \n"; break;}
                bloomRes= proc[monitorFrom].bloomStatus(virus,id);
                if(bloomRes==-1){std::cout<<"VIRUS NOT IN DATABASE \n"; break;}
                if(bloomRes==0)std::cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                else{
                    myWrite(bufSize,fd1[monitorFrom],&act,(char*)"int");
                    myWrite(bufSize,fd1[monitorFrom],(char*)id.c_str(),(char*)"cha");
                    myWrite(bufSize,fd1[monitorFrom],(char*)virus.c_str(),(char*)"cha");
                    myWrite(bufSize,fd1[monitorFrom],(char*)date.c_str(),(char*)"cha");
                    //nespresso what else?
                }
                badFlag=false;
                break;
            case 2:
                if(!(*iss>>id)) break;
                if((*iss>>word))break;


                badFlag=false;
                break;
            case 3:
            //exit
                  for(i=0; i<n; i++){

                     myWrite(bufSize,fd1[i],&act,(char*)"int");
                  }
                exitFlag=true;
                badFlag=false;
                break;
            case 4:
                break;
        }//swich
        if(badFlag) std::cout << "INCORRECT DATA GIVEN" << '\n';
        delete iss;
    }//while exite flag
    for(i=0; i<n; i++){
        close(fd1[i]);
        close(fd2[i]);
        strcpy(mypipe1,(std::to_string(i)+"out" ).c_str());
        strcpy(mypipe2,(std::to_string(i)+"in" ).c_str());
        remove(mypipe1);
        remove(mypipe2);
    }
    delete[] proc;
    delete filesName;
    delete[] fd1;
    delete[] fd2;
    delete[] pids;
    return 0;
}
