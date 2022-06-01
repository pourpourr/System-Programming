#include <iostream>
#include "hashTemplate.h"
#include <string>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <cstring>
#include <dirent.h>
#include "fileHandler.h"
#include "records.h"
#include <pthread.h>
#define perror2(s,e) fprintf(stderr , "%s: %s\n", s, strerror(e))
#define MSGSIZE  250 // max mess size
#define PROC_VIR 17
int main(int argc, char const *argv[])
{
    


    if(argc!=13) {std::cout<<"wrong num of arguments given \n"; return -1;}
    if(strcmp(argv[1],"-m")!=0 || strcmp(argv[3],"-b")!=0|| strcmp(argv[5],"-c")|| strcmp(argv[7],"-s")!=0|| strcmp(argv[9],"-i")!=0|| strcmp(argv[11],"-t")!=0){
        std::cout << "wrong imput give ./travelMonitorClient –m numMonitors -b socketBufferSize -c cyclicBufferSize -s \n sizeOfBloom -i input_dir -t numThreads" << '\n';
        return -6;

    }

    DIR *d;
    if((d = opendir(argv[10]))==NULL){std::cout<<"bad dir name given\n"; return -5;}
    closedir(d);
    unsigned int bloom=int_checker(std::string(argv[8]));
    int numOfProc=int_checker(std::string(argv[2]));
    int sockBufSize=int_checker(std::string(argv[4]));
    int thrBufSize=int_checker(std::string(argv[6]));
    int numThreads=int_checker(std::string(argv[12]));
    if(bloom<=0 || numOfProc<=0 || sockBufSize<=3 || thrBufSize<1 || numThreads<1){
        std::cout << "invalid bloomsize(>0) or bufferSize(>=4) or NumOfMonitors(>0) given" << '\n';
        return -3;
    }
    int bloomSize=int(atoi(argv[8])/4);

    pid_t* pids=new pid_t[numOfProc];
    int i;
    std::string tempStr;
  
     

    int n = numOfProc;
   

    char message[MSGSIZE];
    char type[4];
    int writeRes,mesSize;

    
    List<std::string>* filesName=NULL;
    std::string dir,diri2;
    diri2=std::string(argv[10]);


    fileGetter(&filesName,(char *)diri2.c_str());
        std::string country;
    processData*  proc = new processData[numOfProc];
    List<std::string>** filesPerProc = new List<std::string>*[numOfProc];
    for (i = 0; i < n; i++)filesPerProc[i]= new List<std::string>;
       
       
       
    i=0;
    bool atLeastNum=false;
    while(!filesName->isEmpty()){
        dir=filesName->popFirst();
        country= unDirectorize(diri2,dir);
        proc[i].insertCountry(country); 
        filesPerProc[i]->addEntry(dir);
        if(++i==n){i=0; atLeastNum=true;}
    }
    if(!atLeastNum)n=i;
    int sock;
    int* port= new int[numOfProc];   
    int* newsock= new int[numOfProc];
    struct sockaddr_in server , client ;
    socklen_t clientlen ;
    struct in_addr ** addr_list ;    //getting Ip...
    struct sockaddr * serverptr =( struct sockaddr *) & server ;
    struct sockaddr * clientptr =( struct sockaddr *) & client ;
    struct hostent * rem ;
    char hostName[60];
    char hostIP[60];
    gethostname(hostName,sizeof(hostName));
    rem=gethostbyname(hostName);
    addr_list = (struct in_addr **) rem ->h_addr_list;
    strcpy(hostIP,inet_ntoa (* addr_list [ 0 ]));
    server.sin_family = AF_INET ;
    server.sin_addr.s_addr = inet_addr ( hostIP) ;

    int opt=1;
    srand(time(NULL));
    int rPORT= rand()%4000+2000;
    for (i = 0; i < n; i++) {
        pids[i]=fork();
        port[i]= rPORT+i;                           
        server.sin_port = htons ( port[i] ) ;
        switch(pids[i]){
                case-1: perror("fork  call"); exit (2);
                case 0: { 

                    if(filesPerProc[i]->Size()==0) std::cout<<" warning no files given to monitor\n";
                
                    std::string temp=  stringListToPaths(filesPerProc[i]);
                    char* args[]={(char *)"./monitorServer",(char *)argv[12],(char*) argv[6],(char*) argv[8],(char*)temp.c_str(),(char*) std::to_string(port[i]).c_str(),(char*) argv[4],  NULL };
                    
                    execvp(args[0],args);
                    
                    break; 
                }
        }
        if ((newsock[i]  = socket(AF_INET, SOCK_STREAM, 0)) < 0) {print("sock create failed"); assert(0);}
        opt=1;
        setsockopt(newsock[i], SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
        while (connect(newsock[i] , serverptr, sizeof(server)) < 0){}

    }

    bool exitFlag=false,badFlag;
    std::string line,word,id,date,countryFrom,countryTo,virus,date1,date2;
    std::istringstream* iss;
    int act=0,monitorFrom,monitorTo,bloomRes;

  
    strcpy(message,"dwse");
    strcpy(type,"cha");
    print("Wait for bloom filters to be taken \n");
   for(i=0; i<n; i++){
        

            myWrite(sockBufSize,newsock[i],message,type);
            print("about to get blooms\n");
            bloomGet(sockBufSize,newsock[i],bloom,&proc[i]);
         

        print("monitor done\n");
        
    }
    print("bloom filters acquired \n");
    int* temp = new int;
    bool wholeWorld;
    bool pass;
    List<Date*>* tempDateL;
    List<Date*>* tempDateLcopy;
    int rejected;
    int all;
    int total=0, total_accepted=0;
    Date* dat;
    Hash<List<Date*>>* requests= new Hash<List<Date*>>(PROC_VIR); 
    while(!exitFlag){

        std::cout << "give stuff" << '\n';
        if (!std::getline(std::cin, line) || line.empty()) badFlag=true;
        iss= new std::istringstream(line);
        (*iss)>>word;
        if(word=="/travelRequest") act=1;
        else if(word=="/travelStats")  act=2;
        else if(word=="/addVaccinationRecords")  act=3;
        else if(word=="/searchVaccinationStatus")  act=4;
        else if(word=="/exit")  act=5;
        else act=6;

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
                for(int i=0; i<n; i++){
                    if(proc[i].isCountryInside(countryFrom)){ monitorFrom=i; break;}
                }
                for(int i=0; i<n; i++){
                    if(proc[i].isCountryInside(countryTo)){ monitorTo=i; break;}
                }
                if(monitorFrom==-1 ){std::cout<<"COUNTRY_FROM NOT IN DATABASE \n"; break;}
                if(monitorTo==-1 ){std::cout<<"COUNTRY_TO NOT IN DATABASE \n"; break;}
                if(!dateValidor(date)) break;
                if(int_checker(id)<0){std::cout<<"BAD ID GIVEN \n"; break;}
                bloomRes= proc[monitorFrom].bloomStatus(virus,id);
                if(bloomRes==-1){std::cout<<"VIRUS NOT IN DATABASE \n"; break;}
                else{
                    total++;
                    pass=false;
                    if(bloomRes==0) std::cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED\n";                        
                    else{
                        *temp= act;
                        myWrite(sockBufSize,newsock[monitorFrom],temp,(char*)"int");
                        strcpy(message,(char*)id.c_str());
                        myWrite(sockBufSize,newsock[monitorFrom],message,(char*)"cha");
                        strcpy(message,(char*)virus.c_str());
                        myWrite(sockBufSize,newsock[monitorFrom],message,(char*)"cha");
                        strcpy(message,(char*)date.c_str());
                        myWrite(sockBufSize,newsock[monitorFrom],(char*)date.c_str(),(char*)"cha");
                        //nespresso what else?
                        myRead(sockBufSize,newsock[monitorFrom],message);
                        if(strcmp(message,"accepted")==0)pass=true;
                    }
                    tempDateL= requests->searchInHash(virus);
                    if(tempDateL==NULL){
                        tempDateL = new List<Date*>;
                        requests->insertInHash(virus,tempDateL);
                    }
                    dat= new Date(date,countryTo,pass);
                    tempDateL->addEntry(dat);
                    if(pass)total_accepted++;
                }
                badFlag=false;
                break;
            case 2: 
            //travelStats virusName date1 date2 [country]
                country="";
                wholeWorld=false;
                if(!(*iss>>virus)) break;
                if(!(*iss>>date1)) break;
                if(!(*iss>>date2)) break;
                if(!(*iss>>word)) wholeWorld= true;
                else{
                    country=word;
                    if((*iss>>word)) country="bad news";
                }
                if(country=="bad news")break;


                tempDateL= requests->searchInHash(virus);
                if(tempDateL==NULL){std::cout<<"VIRUS NOT IN DATABASE \n"; break;}
                tempDateLcopy= new List<Date*>;
                tempDateL->ListCopy(tempDateLcopy);
                all=0;
                rejected=0;
                if(wholeWorld){
                    while(!tempDateLcopy->isEmpty()){
                        dat=tempDateLcopy->popFirst();
                        if(dateChecker(date1,date2,dat->getDate())){
                            all++;
                            if(!dat->getAcc())rejected++;
                        }
                    }
                }else{
                    while(!tempDateLcopy->isEmpty()){
                        dat=tempDateLcopy->popFirst();
                        if(dateChecker(date1,date2,dat->getDate()) && dat->getCountry()==country){
                            all++;
                            if(!dat->getAcc())rejected++;
                        }
                    }
                }
                delete tempDateLcopy;
                print("\n");
                print(virus+"\n");
                if(wholeWorld)print("WHOLE WORLD STATS \n");
                else print(country+" \n");
                print("TOTAL REQUESTS "); print(all);
                print("ACCEPTED "); print(all-rejected);
                print("REJECTED "); print(rejected);
                badFlag=false;
                break;
            case 3:
            //addVaccinationRecords Albania
                if(!(*iss>>country)) break;
                if((*iss>>word)) break;
                monitorFrom=-1;
                for(int i=0; i<n; i++){
                    if(proc[i].isCountryInside(country)){ monitorFrom=i; break;}
                }
                if(monitorFrom==-1 ){std::cout<<"COUNTRY FROM NOT IN DATABASE \n"; break;}
                strcpy(type,"int");
                *temp= act;
                
                myWrite(sockBufSize,newsock[monitorFrom],temp,type);
                country=diri2+country;
                strcpy(message,country.c_str());
                strcpy(type,"cha");
                myWrite(sockBufSize,newsock[monitorFrom],message,type);
                strcpy(message,"bloom return");
                strcpy(type,"cha");
                myWrite(sockBufSize,newsock[monitorFrom],message,type);
                print("about to get blooms\n");
                proc[monitorFrom].deleteBloom();
                bloomGet(sockBufSize,newsock[monitorFrom],bloom,&proc[monitorFrom]);
                print("blooms aquired\n");

                badFlag=false;
                break;
            case 4:
            //searchVaccinationStatus 5712
                if(!(*iss>>id)) break;
                if((*iss>>word)) break;
                if(int_checker(id)<0)break;
                for(i=0; i<n; i++){
                    *temp= act;
                     myWrite(sockBufSize,newsock[i],temp,(char*)"int");
                     *temp= int_checker(id);
                     myWrite(sockBufSize,newsock[i],temp,(char*)"int");
                }
                monitorFrom=-1;
                for(i=0; i<n; i++){

                     myRead(sockBufSize,newsock[i],message);
                     if(strcmp(message,"found")==0)monitorFrom=i;
                }
                if(monitorFrom==-1)print("ID "+ id+" NOT IN DATABASE\n");
                else{
                    myRead(sockBufSize,newsock[monitorFrom],message); // record;
                    print(message);
                    print("\n");
                    while (myRead(sockBufSize,newsock[monitorFrom],message)==2)
                    {
                        print(message);
                        print("\n");
                    }
                    
                }
                badFlag=false;
                break;
            case 5: 
            //exit
                  for(i=0; i<n; i++){

                     myWrite(sockBufSize,newsock[i],&act,(char*)"int");
                  }
                exitFlag=true;
                badFlag=false;
                break; 
            case 6:
                break;  
        }//swich
        if(badFlag) std::cout << "INCORRECT DATA GIVEN" << '\n';
        delete iss;
    }//while exite flag  



    std::ofstream outfile ("log"+std::to_string(getpid())+".txt");
    List<std::string>* tempCountries;
    for(i=0; i<n;i++){
        tempCountries=proc[i].getCountries();
        while(!tempCountries->isEmpty()){
            outfile << tempCountries->popFirst() << std::endl;
        }
    }
    outfile << "TOTAL REQUESTS " << total<< std::endl;
    outfile << "ACCEPTED " << total_accepted<<std::endl;
    outfile << "REJECTED " << total-total_accepted<<std::endl;

    outfile.close();







    delete temp;
    List<Date*>* toDel;
    List<List<Date*>*>* l= requests->asList();
    while(!l->isEmpty()){
        toDel=l->popFirst();
        while(!toDel->isEmpty()){
            dat=toDel->popFirst();
            delete dat;
        }

    }
    delete l;
    delete requests;
    for (i = 0; i < n; i++) close(newsock[i]);
    for(i=0; i<numOfProc; i++)delete filesPerProc[i];
    delete[] filesPerProc ;
    delete filesName;
    delete[] pids;
    delete[] newsock;
    delete[] port;
    delete[] proc;
    return 0;
}
