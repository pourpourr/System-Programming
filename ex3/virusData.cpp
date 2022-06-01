#include "virusData.h"

Virus::Virus(std::string vir, unsigned int bloomSize){

  //  std::cout<<virssss++<<" new vir\n";
    this->virus=vir;
    this->vaccinated= new Hash<Patient>(PAT_SIZE);
    this->nonVaccinated= new Hash<Patient>(PAT_SIZE);
    this->vaccBloom=new BloomFilter(bloomSize);
}



Virus::~Virus(){
        //       std::cout<<virssss--<<" deleted vir\n";
    delete this->vaccBloom;
    delete this->vaccinated;
    delete this->nonVaccinated;
}
void Virus::insertPatient(Patient* pat){

    if (pat->getDate()=="-1") this->nonVaccinated->insertInHash(pat->strId(),pat);
    else {
        this->vaccinated->insertInHash(pat->strId(),pat);
        this->vaccBloom->wordInsert((char *)pat->strId().c_str());
    }
}

bool Virus::searchPatient(std::string id){
    Patient* pat1= this->vaccinated->searchInHash(id);
    Patient* pat2=this->nonVaccinated->searchInHash(id);
    return pat1==NULL&&pat2==NULL;
}

Patient* Virus::getVaccPatRecord(std::string id){
    return this->vaccinated->searchInHash(id);
}

Patient::Patient(Record* r , std::string d){
    this->rec=r;
    this->date=d;

}
Patient::~Patient(){
}


processData::processData(){
    this->countries=new List<std::string>;
    this->virBlooms=new Node<BloomFilter>;
}

processData::~processData(){
    delete this->countries;
    delete this->virBlooms;
    
}

void processData::insertCountry(std::string count){
    this->countries->addEntry(count);
}

void processData::insertBloom(std::string virus, BloomFilter* b){
    this->virBlooms->insert(virus,b);
}

void processData::deleteBloom(){
    if(this->virBlooms!=NULL)delete this->virBlooms;
    this->virBlooms=new Node<BloomFilter>;
}


void processData::printBlooms(){
    List<BloomFilter*> *l=this->virBlooms->asList();
    while(!l->isEmpty()){
        BloomFilter* b= l->popFirst();
        b->printBloom();
    }
    delete l;
}

bool processData::isCountryInside(std::string country){
    return this->countries->isInside(country); 
}

int processData::bloomStatus(std::string virus, std::string id){
    BloomFilter* found= this->virBlooms->search(virus);
    if(found==NULL) return -1; //virus not in database
    
    return found->wordSearch((char*)id.c_str());
}

