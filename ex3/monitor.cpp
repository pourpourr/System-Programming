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
#include <netdb.h>	         /* gethostbyaddr */
#include <netinet/in.h>	 
#include <arpa/inet.h>
#include "records.h"
#include "fileHandler.h"
#include <pthread.h>
#define MSGSIZE  250




int main(int argc, char const *argv[]){
    if(argc!=7) {std::cout<<"wrong num of arguments given \n"; return -1;}
    char buf[MSGSIZE];
    unsigned int bloom=int_checker(std::string(argv[3]));
    int thrBufSize=int_checker(std::string(argv[2]));
    int numThreads=int_checker(std::string(argv[1]));
    int port=int_checker(std::string(argv[5]));
    int sockBufSize=int_checker(std::string(argv[6]));
    int sock;

    struct sockaddr_in server;
    socklen_t serverlen=sizeof server  ;

    struct sockaddr *serverptr = (struct sockaddr*)&server;

    struct hostent *rem;
    bool endFlag=false;

    char message[MSGSIZE];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {print("sock create failed"); assert(0);}
    int opt=1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    char hostName[60];
    char hostIP[60];
    struct in_addr ** addr_list ;

     gethostname(hostName,sizeof(hostName)); //getting current ip
    rem=gethostbyname(hostName);
    addr_list = (struct in_addr **) rem ->h_addr_list;
    strcpy(hostIP,inet_ntoa (* addr_list [ 0 ]));
    server.sin_family = AF_INET ;
    server.sin_addr.s_addr = inet_addr ( hostIP) ;
    server.sin_port = htons(port); 

    if ( bind ( sock , serverptr , sizeof ( server ) ) < 0) {perror("bind "); assert(0);}
    if ( listen ( sock , 5) < 0) {print ( " listen " ) ; assert(0);}

    if ((sock= accept(sock, serverptr, &serverlen)) < 0) {perror("accept ") ; assert(0);}

    std::string* temp;
    char type[4];
    List<std::string>* filesName=NULL;
    filesName= stringPathsToList(std::string(argv[4]));

    Hash<Record>* recs =NULL;
    Hash<Virus>* virs =NULL;
    Node<int>* lastFilePos=new Node<int>;
    List<std::string>* countries=new List<std::string> ;
    pthread_t thr;
    int err , exit_status;
    fileGetArgs thr_args;
    thr_args.filesName=&filesName;
    thr_args.lastFilePos=&lastFilePos;
    thr_args.recs=&recs;
    thr_args.virs=&virs;
    thr_args.countries=&countries;
    thr_args.numThreads=numThreads;
    thr_args.thrBufSize=thrBufSize;
    thr_args.bloomSize=bloom;

    if(err=pthread_create(&thr,NULL,mamaThreadFileGetter,(void *) &thr_args ));
    pthread_join(thr,NULL);

    print("threads done\n");


    int act;
    int all=0,rejected=0;
    bool badFlag;
    endFlag=false;
    std::string id,virus,date;

    if(myRead(sockBufSize,sock,message)!=2 ){std::cout<<"expected sth else \n"; return -1;}
    bloomReturn(sockBufSize,sock,int(bloom/4),&virs);


    endFlag=false; 
    while(!endFlag){

      if(myRead(sockBufSize,sock,message)!=1){std::cout<<"expected int but read char \n"; return -1; }

      act=atoi(message);
      badFlag=true;
      bool res;
      Virus* v;
      Patient* p;
      int int_id;
      Record* checkRec;
      List<Virus*> *virList;
      Virus* tempVir;
      Patient* tempPat;
      switch(act) {

          case 1: // requests 
                if(myRead(sockBufSize,sock,message)!=2){std::cout<<"expected to read char \n"; return -1;}
                id=std::string(message);
                if(myRead(sockBufSize,sock,message)!=2){std::cout<<"expected to read char \n"; return -1;}
                virus=std::string(message);
                if(myRead(sockBufSize,sock,message)!=2){std::cout<<"expected to read char \n"; return -1;}
                date=std::string(message);
                v=virs->searchInHash(virus);
                //we know virus is in hash from main
                p= v->getVaccPatRecord(id);
                all++;
                if(p==NULL){
                  
                    std::cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                    rejected++;
                    strcpy(message,"not");
                    strcpy(type,"cha");
                    myWrite(sockBufSize,sock,message,type);
                  }
                else{
                  res=dateChecker(p->getDate(),generateSixMonthsForward(p->getDate()),date);
                  if(res){
                    std::cout<<"REQUEST ACCEPTED – HAPPY TRAVELS\n";
                    strcpy(message,"accepted");
                    strcpy(type,"cha");
                    myWrite(sockBufSize,sock,message,type);
                  }
                  else {
                    rejected++;
                    std::cout<<"REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE \n";
                    strcpy(message,"not");
                    strcpy(type,"cha");
                    myWrite(sockBufSize,sock,message,type);
                  }
                }
                badFlag=false;
                break;

          case 3:
                if(myRead(sockBufSize,sock,message)!=2){std::cout<<"expected to read char \n"; return -1;}
                print(message);
                print("\n");
                if(filesName!=NULL) delete filesName;
                filesName=new List<std::string>;
                filesName->addEntry(std::string(message));
                thr_args.filesName=&filesName;
                thr_args.sock=sock;
                thr_args.sockBufSize=sockBufSize;

                if(err=pthread_create(&thr,NULL,mamaAddVaccinations,(void *) &thr_args ));
                pthread_join(thr,NULL);

                
                badFlag=false;
                break;
          case 4:
                if(myRead(sockBufSize,sock,message)!=1){std::cout<<"expected to read int \n"; return -1;}
                int_id=std::stol(message);
                id=std::string(message);
                checkRec=recs->searchInHash(id);
                if(checkRec==NULL){
                  strcpy(message,"not found");
                  strcpy(type,"cha");
                  myWrite(sockBufSize,sock,message,type);
                }else{
                  strcpy(type,"cha");
                  strcpy(message,"found");
                  myWrite(sockBufSize,sock,message,type);
                  strcpy(message, checkRec->toString().c_str());
                  myWrite(sockBufSize,sock,message,type);
                  virList= virs->asList();
                  while(!virList->isEmpty()){
                    tempVir=virList->popFirst();
                    if(!tempVir->searchPatient(id)){//id has this vir data
                        tempPat=tempVir->getVaccPatRecord(id);
                        if(tempPat!=NULL)strcpy(message,(tempVir->getVir()+" VACCINATED ON "+tempPat->getDate()).c_str());
                        else strcpy(message,(tempVir->getVir()+" NOT YET VACCINATED").c_str()); 
                        myWrite(sockBufSize,sock,message,type);
                    }

                    
                  }
                  strcpy(type,"int");
                  myWrite(sockBufSize,sock,&opt,type);
                  delete virList;
                }




                badFlag=false;
                break; 
            
          case 5:
                endFlag=true;
                badFlag=false;
                break; 
      }//switch
      if(badFlag) std::cout << "INCORRECT DATA GIVEN in monitor" << '\n';
  }//while
  

   std::ofstream outfile ("log"+std::to_string(getpid())+".txt");

    while(!countries->isEmpty()){
        outfile << countries->popFirst() << std::endl;
    }
    
    outfile << "TOTAL REQUESTS " << all<< std::endl;
    outfile << "ACCEPTED " << all-rejected<<std::endl;
    outfile << "REJECTED " << rejected<<std::endl;

    outfile.close();
  
  
  
  
  
  
  
 
   
 
   close(sock);
   if(filesName!=NULL) delete filesName;
   if(countries!=NULL) delete countries;
   if(virs!=NULL) delete virs;
   if(recs!=NULL) delete recs;
   if(lastFilePos!=NULL) delete lastFilePos;
  
    print("monitor ok\n");
         
    return 0;
}
