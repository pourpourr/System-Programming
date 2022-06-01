#include "records.h"

Record::Record(std::string str_id,std::string name,std::string surname,std::string country,std::string str_age){
    this->id=stoi(str_id);
    this->age=stoi(str_age);
    this->name=name;
    this->surname=surname;
    this->country=country;
    this->next=NULL;
}

Record::~Record(){
    if (this->next!=NULL) delete this->next;
}

void Record::printRec(){
    std::cout << this->id<<" "<<this->name<<" "<<this->surname<<" "<<this->country<<" "<<this->age<<" "<<'\n';
    //if (this->next!=NULL) (this->next)->printRec();
}

Record* Record::addRec(Record* rec ){
    if(rec->id==this->id){
        if(rec->name==this->name&&rec->surname==this->surname&&rec->country==this->country&&rec->age==this->age)
            return this; // returning this means record already with consistent the other elements( name , surname etc)
        else return NULL; // null means inconsistent
    }else{
        if(this->next!=NULL) return (this->next)->addRec(rec);
        else {
            this->next=rec;
            return this->next;
        }
    }
}

Record* Record::searchRec(int rId ){
    if(this->id==rId) return this;
    if(this->next==NULL) return NULL;
    return (this->next)->searchRec(rId);
}

RecordHash::RecordHash(int size){

    int i;
    this->hashSize=size;
    this->recsTable=new Record*[hashSize];
    for(i=0; i<hashSize; i++)  this->recsTable[i]=NULL;

}

RecordHash::~RecordHash(){
    int i;
    for(i=0; i<this->hashSize; i++)  delete this->recsTable[i];
    delete[] this->recsTable;
}

Record* RecordHash::addEntry(Record* rec){
    char id[12];
    sprintf(id, "%d", rec->id);
    int key=djb2((unsigned char*)id)%this->hashSize;
    if (this->recsTable[key]==NULL) {
        this->recsTable[key]=rec;
        return this->recsTable[key];
    }else return (this->recsTable[key])->addRec(rec);

}

Record* RecordHash::searchEntry(int rId){
    char id[12];
    sprintf(id, "%d", rId);
    int key=djb2((unsigned char*)id)%this->hashSize;
    if (this->recsTable[key]==NULL) return NULL;
    else return (this->recsTable[key])->searchRec(rId);
}

void RecordHash::printHash(){
    for(int i=0; i<this->hashSize; i++) {
        std::cout << "hash pos == "<<i << '\n';
        if(this->recsTable[i]!=NULL) (this->recsTable[i])->printRec();
    }
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
