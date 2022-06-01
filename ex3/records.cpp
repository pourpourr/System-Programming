#include "records.h"

Record::Record(int id,std::string name,std::string surname,std::string country,int age){
    this->id=id;
    this->age=age; //balto sth main
    this->name=name;
    this->surname=surname;
    this->country=country;
     //   std::cout<<reee++<<" new rec\n";

}

Record::~Record(){
          //                  std::cout<<reee--<<" deleted rec\n";

        }
void Record::printRec(){
    std::cout << this->id<<" "<<this->name<<" "<<this->surname<<" "<<this->country<<" "<<this->age<<" "<<'\n';
}