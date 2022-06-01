#ifndef __RECS__
#define __RECS__

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <ctime>

static int reee=0;
class Record
{
    private:
        int id;
        std::string name;
        std::string surname;
        std::string country;
        short int age;
    public:
        Record(int id,std::string name,std::string surname,std::string country,int age);
        ~Record();
        void printRec();
        int getId(){
            return this->id;
        }
        int getAge(){
            return this->age;
        }
        std::string getCountry(){
            return this->country;
        }
        std::string toString(){
            return "\n"+std::to_string(id)+" "+name+" "+surname+" \n"+country+" "+std::to_string(age)+"\n";
        }
        bool compare(Record* rec){
            return (this->id==rec->id)&&(this->name==rec->name)&&(this->surname==rec->surname)&&(this->country==rec->country)&&(this->age==rec->age);
        }
};


#endif