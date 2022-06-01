#include "fileHandler.h"

List<std::string>* fileGetter(List<std::string>* listoula, char* dirName){
      DIR *d;
      if((d = opendir(dirName))==NULL)std::cout<<"bad dir name given\n";
      std::string textaki;
      struct dirent * dp;
      if(listoula==NULL)listoula= new List<std::string>;
      while ((dp = readdir(d)) != NULL) {

          textaki=std::string(dp->d_name);
          std::size_t found= textaki.find(".txt");
          if (found!=std::string::npos){
            if(textaki!="." && textaki!="..")listoula->addEntry(std::string(dirName)+"/"+textaki); }
        else{
            if(textaki!="." && textaki!="..")listoula->addEntry(std::string(dirName)+textaki);}

          
      }
      closedir(d);
      return listoula;
}

void dataInsert(Hash<Record>** recs, Hash<Virus>** virs , List<std::string>** filesName, int bloomSize){
    
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
    while(!(*filesName)->isEmpty()){
        std::string fn=(*filesName)->popFirst();
        file.open(fn);
        
        badFlag=false;
        empty=true;
        while (file >> word){
            empty=false;
            datChanged=false;
            if (word.length()<=4) { //word is id . We take as granded that is ids 
                                    // are at most 4 digit nums, from the previus assigment
                                    //eisagwgh prohgoumenhs
                if(surname!=""){


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
                            }else {std::cout << "ERROR WITH CITIZEN "<< id <<" FROM " <<country<<" ALREADY IN "<<virus<<" DATABASE"<<'\n'; /*delete cherec;*/}
                               
                            
                        }else{std::cout << "ERROR WITH CITIZEN "<< id <<" : INCONSISTENT DATA "<<'\n'; delete rec;}

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
                std::cout << "ERROR WITH CITIZEN "<< id << '\n';
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
                        }else {std::cout << "ERROR WITH CITIZEN "<< id <<" FROM " <<country<<" ALREADY IN "<<virus<<" DATABASE"<<'\n'; /*delete cherec;*/}
                            
                        
                    }else{std::cout << "ERROR WITH CITIZEN "<< id <<" FROM " <<country<<" : INCONSISTENT DATA "<<'\n'; delete rec;}


              //  }
            }else std::cout << "ERROR WITH CITIZEN "<< id << '\n';
        }
        file.close();
        id="";
        name="";
        surname="";
        country="";
        age="";
        virus="";
        ans="";
        date="";
        word="";
    }

}

void bloomReturn(int bufSize , int fdWrite ,int bloomSize, Hash<Virus>** virs ){
    unsigned int opa;
    List<Virus*>* virList =(*virs)->asList();
    Virus* vir;
    BloomFilter** b;
    char type[4];
    while (!virList->isEmpty()){
        vir=virList->popFirst();

        strcpy(type,"cha");
        myWrite(bufSize,fdWrite,(char*)((vir->getVir()).c_str()),type);
        b=vir->getBloom();
        for(int i=0; i<bloomSize; i++){
            opa=(**b).getNumInPos(i);
            strcpy(type,"int");
            myWrite(bufSize,fdWrite,&opa,type);
        }
    }
    strcpy(type,"cha");

    myWrite(bufSize,fdWrite,(char*)"no more bloom",type);
    delete virList;
    
        
}

void bloomGet(int bufSize , int fdRead,int bloomSize, processData* pD){
    unsigned int opa;
    BloomFilter* b;
    std::string virName;
    char message[MSGSIZE];
    int readRes=myRead(bufSize,fdRead,message);
    int realBloomSize= int(bloomSize/4);
    if(readRes!=2)std::cout<<"expected char from myRead in bloomGet \n";   
    else{
        while (readRes==2 && strcmp(message,"no more bloom")!=0){ // here we read the virus and the bloom
            virName=std::string(message);
            b=new BloomFilter(bloomSize);

            for(int i=0; i<realBloomSize; i++){
                myRead(bufSize,fdRead,message);
                opa=std::stol((std::string(message)));
                b->setNumInPos(i , opa);
            }

            pD->insertBloom(virName,b);
            readRes=myRead(bufSize,fdRead,message);
        }
    }
}


int myRead(int bufSize, int fdRead, char* message ){
    int mesSize;
    char messType[4];
    memset(messType,0,4);
    char* temp= new char[bufSize];
    while(read(fdRead,messType,4)==0){}
    if(strcmp(messType,"int")!=0&&strcmp(messType,"cha")!=0){std::cout<<"bad type entered myRead\n";return -2;}

    read(fdRead,&mesSize,4);
   int iterations= mesSize/(bufSize-1);
    if(mesSize%(bufSize-1)!=0)iterations++;
        memset(message,0,MSGSIZE);

     if(iterations-1>0){
        for(int i=0; i<iterations-1; i++){
            memset(temp,0,bufSize);
        
            read(fdRead,temp,bufSize);

            strcat(message,temp);

        }
     }

     int leftbytes= mesSize-1-(iterations-1)*(bufSize-1);

     if(leftbytes>0){   
        read(fdRead,temp,leftbytes+1);

        strcat(message,temp);
     }
    delete[] temp;
    if(strcmp(messType,"int")==0) return 1;
    if(strcmp(messType,"cha")==0) return 2;
    return -3;
}



int myWrite(int bufSize,int fdWrite,void* message,char* messType ){ 
    if(strcmp(messType,"int")!=0&&strcmp(messType,"cha")!=0){std::cout<<"bad type entered myWrite\n";return -2;}
    write(fdWrite,messType,4); //writing the type

    char* mess;
    if(strcmp(messType,"int")==0){
        mess= new char[12*sizeof(char)];
        strcpy(mess,(std::to_string(*(unsigned int* )message)).c_str());
        
    }else {
        char* temp2= (char*)message;
        mess= new char[(strlen(temp2)+1)*sizeof(char)];
        strcpy(mess,temp2);
    }
    int mesSize=strlen(mess)+1;
    write(fdWrite,&mesSize,4); //writing the sizes

    int iterations= mesSize/(bufSize-1);
    if(mesSize%(bufSize-1)!=0)iterations++;


    char* temp= new char[bufSize];
    int j,i;
    for( i=0 ; i<iterations-1;i++){

        
        memset(temp,0,bufSize);

        for( j=0; j<bufSize-1; j++){
            temp[j]=mess[j+i*(bufSize-1)];
        }
        temp[j]='\0';

       
        write(fdWrite,temp,bufSize);
        
    }

    int leftbytes= mesSize-1-(iterations-1)*(bufSize-1);
    if(leftbytes>0){
        memset(temp,0,bufSize);
        for( j=0; j<leftbytes; j++){
                temp[j]=mess[j+(iterations-1)*(bufSize-1)];
        }
        temp[j]='\0';

        write(fdWrite,temp,leftbytes+1);
    }
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
    std::cout<<future<<"/n";
    future=generateSixMonthsBack(future);
        std::cout<<future<<"/n";

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

std::string unDirectorize(std::string mama, std::string  dir){
    
    char temp[50];
    int i;
    
    for( i=0; i<strlen(dir.c_str())-strlen(mama.c_str())+1; i++){
        temp[i]=dir[i+strlen(mama.c_str())];
    }
   // temp[i]='/0';
    
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
