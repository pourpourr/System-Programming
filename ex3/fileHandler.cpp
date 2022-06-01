#include "fileHandler.h"
#include <pthread.h>


void print(std::string str){
    std::cout<<str;
}

void print(unsigned int num){
    std::cout<<num<<std::endl;
}

std::string stringListToPaths(List<std::string>* l){
    if(l->isEmpty())return "";
    std::string temp,path;
    while(!l->isEmpty()){
        temp=l->popFirst();
        path=path+" "+temp;
    }
    return path;
}

List<std::string>*  stringPathsToList(std::string paths){
    std::istringstream* iss;
    std::string word;
    List<std::string>* l = new List<std::string>;
    iss= new std::istringstream(paths);
    while((*iss)>>word) l->addEntry(word);

    delete iss;
    return l;
}


pthread_mutex_t   buffer_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t   cond_mama ;
pthread_cond_t   cond_kid ;

bool filesEnd;
bool end;

void  *mamaThreadFileGetter ( void * args){
    fileGetArgs* ar= (fileGetArgs* ) args;
    int n = ar->numThreads;
    int i;
    Node<int>* lastFilePos= *(ar->lastFilePos);
    List<std::string> *countries= *(ar->countries);
    List<std::string>* countriesFiles= new List<std::string>; // all the files that are gonna
                                                            // be distributed within the threads
    List<std::string>* temp=NULL;
    std::string dir,country;

    i=0;
    int* numOfTxts;   //seperating the paths to txt...
    while(!(*ar->filesName)->isEmpty()){
        dir=(*ar->filesName)->popFirst();
        numOfTxts= new int;
        *numOfTxts=fileGetter(&temp,(char*)dir.c_str());

        country=unDirectorize(unDirectorize2(dir),dir);
        countries->addEntry(country);
        lastFilePos->insert(country,numOfTxts);
        countriesFiles->ListConCat(temp);
        temp=NULL;
    }

    if(countriesFiles->isEmpty()){
        std::cout<<"NO FILES READ RETURNING\n";
    }else{
        List<std::string> * filesBuffer= new List<std::string>;
        int bufSize= ar->thrBufSize;
        if(countriesFiles->Size()<bufSize) bufSize=countriesFiles->Size(); 
        if(countriesFiles->Size()<n) n=countriesFiles->Size();  // n is num of threads..



        for(i=0; i<bufSize; i++){
            filesBuffer->addEntry(countriesFiles->popFirst());
        }


        pthread_t* thr= new pthread_t[n];
        int err;
      

        end=false;
        pthread_cond_init (& cond_kid , 0);
        pthread_cond_init (& cond_mama , 0);
        pthread_mutex_init(&buffer_lock ,0);
        fileGetArgs* arKid= new fileGetArgs;
        arKid->bloomSize=ar->bloomSize;
        arKid->filesName=&filesBuffer; //we pass the buffer not all the files to the kid
        arKid->recs=ar->recs;
        arKid->virs=ar->virs;
        arKid->thrBufSize=0;
        for(i=0; i<n; i++){

            if(err=pthread_create(&thr[i],NULL,kidThreadFileGetter,(void*)arKid ));

        }

        pthread_mutex_lock (& buffer_lock);

        filesEnd=false;

        while(!filesEnd){
            while(filesBuffer->Size()==bufSize )pthread_cond_wait(& cond_mama , & buffer_lock);

            if(!countriesFiles->isEmpty()){
                filesBuffer->addEntry(countriesFiles->popFirst());
            }
            if(filesBuffer->isEmpty()&&countriesFiles->isEmpty()){filesEnd=true;break;}
            pthread_mutex_unlock (& buffer_lock);
            pthread_cond_signal (& cond_kid);

            pthread_mutex_lock (& buffer_lock);


        }
        pthread_mutex_unlock (& buffer_lock);
        end=true;
        pthread_cond_broadcast (& cond_kid);

        for(i=0; i<n; i++) {
       
            pthread_join(thr[i],NULL);
        }
        
        if(err=pthread_mutex_destroy (&buffer_lock)){perror("mutex destroy "); assert(0);}
        if(err=pthread_cond_destroy (&cond_kid)){perror("cond destroy "); assert(0);}
        if(err=pthread_cond_destroy (&cond_mama)){perror("cond destroy "); assert(0);}


        delete[] thr;
        delete filesBuffer;
        delete arKid;
       
    }

    delete countriesFiles;
        pthread_exit(NULL);
    return 0;

}
void *kidThreadFileGetter(void * args){
      fileGetArgs* ar= (fileGetArgs* ) args;
    int i;
    List<std::string> * filesBuffer= *(ar->filesName);


    pthread_mutex_lock (& buffer_lock);
    std::string fn;
    while(!filesEnd){
        if(!filesBuffer->isEmpty()){ fn=filesBuffer->popFirst(); dataInsert(ar->recs,ar->virs,ar->bloomSize,fn);}



         pthread_mutex_unlock (& buffer_lock);

         pthread_mutex_lock (& buffer_lock);
        pthread_cond_signal (& cond_mama);

        if(end)break;

       pthread_cond_wait (& cond_kid, &buffer_lock);

    }

            pthread_mutex_unlock (& buffer_lock);


    pthread_exit(NULL);
    return 0;
}

void  *mamaAddVaccinations ( void * args){
    fileGetArgs* ar= (fileGetArgs* ) args;
    int n = ar->numThreads;
    int i;
    Node<int>* lastFilePos= *(ar->lastFilePos);
    std::string country,dir= (*ar->filesName)->popFirst();
    List<std::string>* temp=NULL;
    country=unDirectorize(unDirectorize2(dir),dir);
    
    int filesBefore= *(lastFilePos->search(country));
    int filesNow= fileGetter(&temp,(char*)dir.c_str());
    *(lastFilePos->search(country))=filesNow;
    
    //we trust the user
    List<std::string>* countriesFiles= new List<std::string>;
    for( i=filesBefore+1;i<filesNow+1; i++){
        countriesFiles->addEntry(dir+"/"+country+"-"+std::to_string(i)+".txt");
    }
    List<std::string> * filesBuffer= new List<std::string>;
    int bufSize= ar->thrBufSize;
    countriesFiles->PrintList();
    if(countriesFiles->Size()<bufSize) bufSize=countriesFiles->Size();
    if(countriesFiles->Size()<n)n=countriesFiles->Size();


    for(i=0; i<bufSize; i++){
        filesBuffer->addEntry(countriesFiles->popFirst());
    }
    pthread_t* thr= new pthread_t[n];
    int err;
    end=false;
    
    pthread_cond_init (& cond_kid , 0);
    pthread_cond_init (& cond_mama , 0);
    pthread_mutex_init(&buffer_lock ,0);
    fileGetArgs* arKid= new fileGetArgs;
    arKid->bloomSize=ar->bloomSize;
    arKid->filesName=&filesBuffer; //we pass the buffer not all the files
    arKid->recs=ar->recs;
    arKid->virs=ar->virs;
    for(i=0; i<n; i++){

        if(err=pthread_create(&thr[i],NULL,kidThreadFileGetter,(void*)arKid ));

    }


    pthread_mutex_lock (& buffer_lock);

    filesEnd=false;

    while(!filesEnd){
        while(filesBuffer->Size()==bufSize )pthread_cond_wait(& cond_mama , & buffer_lock);

        if(!countriesFiles->isEmpty()){
            filesBuffer->addEntry(countriesFiles->popFirst());
        }
        if(filesBuffer->isEmpty()&&countriesFiles->isEmpty()){filesEnd=true;break;}

        pthread_mutex_unlock (& buffer_lock);
        pthread_cond_signal (& cond_kid);
        sleep(0.5);
        pthread_mutex_lock (& buffer_lock);


    }

    pthread_mutex_unlock (& buffer_lock);
    end=true;
    pthread_cond_broadcast (& cond_kid);


    for(i=0; i<n; i++) {
    
        pthread_join(thr[i],NULL);
    }
    print("threads done\n");
    if(err=pthread_mutex_destroy (&buffer_lock)){perror("mutex destroy "); assert(0);}
    if(err=pthread_cond_destroy (&cond_kid)){perror("cond destroy "); assert(0);}
    if(err=pthread_cond_destroy (&cond_mama)){perror("cond destroy "); assert(0);}
    char message[MSGSIZE];
    
    myRead(ar->sockBufSize,ar->sock,message);
    print("b4 bloom return (in monitor thread)\n");
    bloomReturn(ar->sockBufSize,ar->sock,int(ar->bloomSize/4),arKid->virs);
    print("after bloom return(in monitor thread)\n");
    delete[] thr;
    delete filesBuffer;
    delete arKid;
    delete temp;
    delete countriesFiles;
    

    return 0;
}







int fileGetter(List<std::string>** listoula, char* dirName){
      DIR *d;
      int filesRead=0;
      if((d = opendir(dirName))==NULL)std::cout<<"bad dir name given\n";
      std::string textaki;
      struct dirent * dp;
      if((*listoula)==NULL)(*listoula)= new List<std::string>;
      while ((dp = readdir(d)) != NULL) {
          filesRead++;
          textaki=std::string(dp->d_name);
          std::size_t found= textaki.find(".txt");
          if (found!=std::string::npos){
            if(textaki!="." && textaki!="..")(*listoula)->addEntry(std::string(dirName)+"/"+textaki); }
        else{
            if(textaki!="." && textaki!="..")(*listoula)->addEntry(std::string(dirName)+textaki);}


      }

      closedir(d);
      return filesRead-2; //we dont count "./", ".//"
}

void dataInsert(Hash<Record>** recs, Hash<Virus>** virs ,  int bloomSize, std::string fn){
    if(*recs==NULL) *recs= new Hash<Record>(RECSIZE);
    if(*virs==NULL) *virs= new Hash<Virus>(VIRSIZE);
    Record* rec;
    Record* checkRec;
    Patient* pat;
    Virus* vir;
    std::fstream file;
    std::string id,name,surname,country,age,virus,ans,date,date1,date2,line,word;
    int int_id,int_age;
    bool badFlag,exitFlag,datChanged,empty;

        badFlag=false;
        empty=true;
        file.open(fn);
        while (file >> word){
            empty=false;
            datChanged=false;
            if (word.length()<=4) { //word is id . We take as granded that is ids
                                    // are at most 4 digit nums, from the previus assigment
                                    //eisagwgh prohgoumenhs
                if(surname!=""){

                        if(int_checker(id)<0 || int_checker(age)<0) {print(id+" =id "+age+" =age "+name+" =name " +virus+surname+"\n"+fn); assert(0);}

                        rec=new Record(stoi(id),name,surname,country,stoi(age));

                        checkRec=(*recs)->searchInHash(id);
                        if(checkRec==NULL){(*recs)->insertInHash(id,rec); }
                        if(checkRec==NULL || checkRec->compare(rec)){ // for this to be true : either first time in hash or consistent second time
                            if(checkRec!=NULL)delete rec;
                            else checkRec=rec;

                            vir=(*virs)->searchInHash(virus);
                            if(vir==NULL){vir=new Virus(virus,bloomSize); (*virs)->insertInHash(virus,vir);} ///////eeeeee blllooooommm k sto apo katw


                            if(vir->searchPatient(id)){
                                pat= new Patient(checkRec,date);
                                vir->insertPatient(pat);
                            }else {/*std::cout << "ERROR WITH CITIZEN "<< id <<" FROM " <<country<<" ALREADY IN "<<virus<<" DATABASE"<<'\n'; /*delete cherec;*/}


                        }else{/*std::cout << "ERROR WITH CITIZEN "<< id <<" : INCONSISTENT DATA "<<'\n';*/ delete rec;}

                }
                id=word;
                file>>name;
                file>>surname;
                file>>country;
                file>>age;
                file>>virus;
                file>>ans;
                if(ans.compare("YES")==0){
                    file>>date;
                    datChanged=true;
                }else date="-1";

            }
            else{ //word is false date
                     //oxi eisagwgh giati bad date ERROR IN RECORD theRecord
               // std::cout << "ERROR WITH CITIZEN "<< id << '\n';
                if(!(file>>word)) {
                    badFlag=true;
                    break;
                }
                id=word;
                file>>name;
                file>>surname;
                file>>country;
                file>>age;
                file>>virus;
                file>>ans;
                if(ans.compare("YES")==0){
                    file>>date;
                    datChanged=true;
                }else date="-1";
            }






        }// while (file >> word)
        if(!empty){
            if(!badFlag){

                    if(int_checker(id)<0 || int_checker(age)<0) {print(id+" =id "+age+" =age "+name+" =name " +virus+surname+"\n"+fn); assert(0);}
                    rec=new Record(stoi(id),name,surname,country,stoi(age));
                    checkRec=(*recs)->searchInHash(id);
                    if(checkRec==NULL)(*recs)->insertInHash(id,rec);
                    if(checkRec==NULL || checkRec->compare(rec)){ // for this to be true : either first time in hash or consistent second time
                        if(checkRec!=NULL)delete rec;
                        else checkRec=rec;

                        vir=(*virs)->searchInHash(virus);
                        if(vir==NULL){vir=new Virus(virus,bloomSize); (*virs)->insertInHash(virus,vir);}
                        if(vir->searchPatient(id)){
                            pat= new Patient(checkRec,date);
                            vir->insertPatient(pat);
                        }else {/*std::cout << "ERROR WITH CITIZEN "<< id <<" FROM " <<country<<" ALREADY IN "<<virus<<" DATABASE"<<'\n'; /*delete cherec;*/}


                    }else{/*std::cout << "ERROR WITH CITIZEN "<< id <<" FROM " <<country<<" : INCONSISTENT DATA "<<'\n'; */delete rec;}


            }else{} /*std::cout << "ERROR WITH CITIZEN "<< id << '\n';*/
        }

        file.close();
 
}

void bloomReturn(int bufSize , int fdWrite ,int bloomSize, Hash<Virus>** virs ){
    //remember bloom/4
    unsigned int *opa= new unsigned int;
    List<Virus*>* virList =(*virs)->asList();
    Virus* vir;
    BloomFilter** b;
    char type[4];
    char virus[30];
    char message[30];
    while (!virList->isEmpty()){
        vir=virList->popFirst();

        strcpy(type,"cha");
        strcpy(virus,(char*)((vir->getVir()).c_str()));
        myWrite(bufSize,fdWrite,virus,type);
        b=vir->getBloom();
        for(int i=0; i<bloomSize; i++){
            *opa=(**b).getNumInPos(i);
            strcpy(type,"int");
            myWrite(bufSize,fdWrite,opa,type);
        }
        myRead(bufSize,fdWrite,message);
    }
    strcpy(type,"cha");
    strcpy(virus,"no more bloom");
    myWrite(bufSize,fdWrite,virus,type);
    delete virList;
    delete opa;


}

void bloomGet(int bufSize , int fdRead,int bloomSize, processData* pD){
    unsigned int* opa= new unsigned int;
    BloomFilter* b;
    std::string virName;
    char message[MSGSIZE];
    char type[4];
    strcpy(message,"dwse");
            strcpy(type,"cha");
    int readRes=myRead(bufSize,fdRead,message);
    int realBloomSize= int(bloomSize/4);
    if(readRes!=2)std::cout<<"expected char from myRead in bloomGet \n";
    else{
        while (readRes==2 && strcmp(message,"no more bloom")!=0){ // here we read the virus and the bloom
            virName=std::string(message);
            b=new BloomFilter(bloomSize);

            for(int i=0; i<realBloomSize; i++){
                myRead(bufSize,fdRead,message);
                try{

                    *opa=std::stol((std::string(message)));
                }catch(std::invalid_argument s){
                    print("edw egine lathos (blooomGet invalid argument)  \n");
                    print(message);
                    print(sizeof(message));
                    
                    sleep(2);
                }
                b->setNumInPos(i , *opa);
            }

            pD->insertBloom(virName,b);
            
            myWrite(bufSize,fdRead,message,type);
            readRes=myRead(bufSize,fdRead,message);
        }
    }
    delete opa;
}


int myRead(int bufSize, int fdRead, char* message ){
    int* mesSize= new int;
    char messType;
    int err;

    char* temp= new char[bufSize];
    while((err=read(fdRead,&messType,1))!=1){}

    if(messType!='i'&& messType!='c'){std::cout<<"bad type entered myRead\n"<<messType<<"\n";assert(0);}
    while((err=read(fdRead,mesSize,4))!=4){}


    int iterations= *mesSize/(bufSize-1);
    if(*mesSize%(bufSize-1)!=0)iterations++;
    strcpy(message,"");

    if(iterations-1>0){
        for(int i=0; i<iterations-1; i++){
            strcpy(temp,"");
        
            while((err=read(fdRead,temp,bufSize))!=bufSize){}

            
            if (err<0){perror("read in iterations"); return -1;}
            

            strcat(message,temp);

        }
    }
    int leftbytes= *mesSize-1-(iterations-1)*(bufSize-1);

    if(leftbytes>0){

         
        strcpy(temp,"");
        
        while((err=read(fdRead,temp,leftbytes+1))!=(leftbytes+1)){}

        if (err<0){perror("read in leftBytes"); return -1;}


        strcat(message,temp);
    }
    delete mesSize;
     
    delete[] temp;
  
    if(messType=='i') return 1;
    if(messType=='c') return 2;
    return -3;
}



int myWrite(int bufSize,int fdWrite,void* message,char* messType ){
    if(strcmp(messType,"int")!=0&&strcmp(messType,"cha")!=0){std::cout<<"bad type entered myWrite\n";return -2;}
    char* mess= new char[MSGSIZE*sizeof(char)];
    strcpy(mess,"");
    char* type = new char;
    *type=messType[0];
    write(fdWrite,type,1);
    if(strcmp(messType,"int")==0){
        strcpy(mess,(std::to_string(*(unsigned int* )message)).c_str());

    }else {
        
        strcpy(mess,(char*)message);
    }
    int* mesSize=new int;
    *mesSize=strlen(mess)+1;

    write(fdWrite,mesSize,4); //writing the sizes

    int iterations= *mesSize/(bufSize-1);
    if(*mesSize%(bufSize-1)!=0)iterations++;


    char* temp= new char[bufSize];
    int j,i;
    for( i=0 ; i<iterations-1;i++){

        strcpy(temp,"");

        for( j=0; j<bufSize-1; j++){
            temp[j]=mess[j+i*(bufSize-1)];
        }
        temp[j]='\0';


        write(fdWrite,temp,bufSize);

    }

    int leftbytes= *mesSize-1-(iterations-1)*(bufSize-1);
    if(leftbytes>0){
        strcpy(temp,"");

        for( j=0; j<leftbytes; j++){
                temp[j]=mess[j+(iterations-1)*(bufSize-1)];
        }
        temp[j]='\0';

        write(fdWrite,temp,leftbytes+1);
    }

    delete type;
    delete mesSize;
    delete[] mess;
    delete[] temp;
    return 0;
}


char* directorize( char* name){
      return (char*)("./"+std::string(name)).c_str();
}


bool dateChecker(std::string date1, std::string date2, std::string real){
    int month,m1,m2;
    int day,d1,d2;
    int year,y1,y2;
    bool greater=false, less=false;
    if(!(dateValidor(date1)&&dateValidor(real)&&dateValidor(date2))){
        return false;
    }
    if (std::sscanf(real.c_str(), "%d-%d-%d", &day, &month, &year) != 3) {
       return false;
    }
    if (std::sscanf(date1.c_str(), "%d-%d-%d", &d1, &m1, &y1) != 3) {
       return false;
    }
    if (std::sscanf(date2.c_str(), "%d-%d-%d", &d2, &m2, &y2) != 3) {
       return false;
    }
    if(year>y1 ||(year==y1&& month>m1)||(year==y1 && month==m1 && day>d1)){
        greater=true;
    }
    if(year<y2 ||(year==y2&& month<m2)||(year==y2 && month==m2 && day<d2)){
        less=true;
    }
    return less&&greater;

}

bool sixMonthsChecker(std::string past , std::string future){
     int m1,m2;
    int d1,d2;
    int y1,y2;
    future=generateSixMonthsBack(future);

    if(!(dateValidor(past)&&dateValidor(future))){
        return false;
    }
    if (std::sscanf(past.c_str(), "%d-%d-%d", &d1, &m1, &y1) != 3) {
       return false;
    }
    if (std::sscanf(future.c_str(), "%d-%d-%d", &d2, &m2, &y2) != 3) {
       return false;
    }
    if(y2>y1 ||(y2==y1&& m2>m1)||(y2==y1 && m2==m1 && d2>=d1)){
        return true;
    }
    return false;

}


std::string generateSixMonthsBack(std::string date){

    if(!dateValidor(date)) return "";
    int d,m,y;
    if (std::sscanf(date.c_str(), "%d-%d-%d", &d, &m, &y) != 3) {
        std::cout << "bad date given (bad syntact )" << '\n';
        return "";
    }
    if(m>6) return std::to_string(d)+"-"+std::to_string(m-6)+"-"+std::to_string(y);
    else return std::to_string(d)+"-"+std::to_string(m+6)+"-"+std::to_string(y-1);
}


std::string generateSixMonthsForward(std::string date){

    if(!dateValidor(date)) return "";
    int d,m,y;
    if (std::sscanf(date.c_str(), "%d-%d-%d", &d, &m, &y) != 3) {
        std::cout << "bad date given (bad syntact )" << '\n';
        return "";
    }
    if(m<6) return std::to_string(d)+"-"+std::to_string(m+6)+"-"+std::to_string(y);
    else return std::to_string(d)+"-"+std::to_string(m-6)+"-"+std::to_string(y+1);
}

std::string unDirectorize2( std::string  dir){
    char temp[50];
    int i;
    char c;
    int timesKathetos=0;
    dir.shrink_to_fit();
    for( i=0; i<dir.size()+1; i++){
        c=dir[i];
        temp[i]=c;
        if(c=='/')timesKathetos++;

        if(timesKathetos==1)break;
    }
    temp[++i]='\0';
    return std::string(temp);
}

std::string unDirectorize(std::string mama, std::string  dir){

    char temp[50];
    int i;

    for( i=0; i<strlen(dir.c_str())-strlen(mama.c_str())+1; i++){
        temp[i]=dir[i+strlen(mama.c_str())];
    }

    return std::string(temp);
}

bool dateValidor(std::string date){
    int d,m,y;
    if (std::sscanf(date.c_str(), "%d-%d-%d", &d, &m, &y) != 3) {
        std::cout << "bad date given (bad syntact )" << '\n';
        return false;
    }
    if(d>0 && d<=31 && m>0 && m<13){
        return true;
    }
    std::cout << "bad date given (crazy nums)" << '\n';
    return false;


}

std::string getTodaysDate(){
    time_t now = time(0);
    bool badFlag=false;
    tm *ltm = localtime(&now);
    int day=ltm->tm_mday ;
    int month= 1 + ltm->tm_mon;
    int year= 1900 + ltm->tm_year;
    char date[14];
    sprintf(date, "%d-%d-%d", day, month, year);
    std::string str(date);
    return str;
}

int int_checker(std::string str_id){
    try{
        int id = stoi(str_id);
        char temp[15];
        sprintf(temp, "%d", id);
        if(strcmp(temp,str_id.c_str())==0)return id;
        else return -1;
    }catch(std::invalid_argument s){
        std::cout << "wrong number input " << '\n';
        return -1;
    }

}
