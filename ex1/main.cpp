#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include "skipList.h"
#include "bloomfilter.h"
#include "records.h"
#include "virusData.h"
#include "countryStats.h"
//#include <filesystem>

//void blooming();


int main(int argc, char const *argv[]){

    if(argc!=5){
        std::cout << "wrong num of arguments" << '\n';
        return -1;
    }
    if(strcmp(argv[1],"-c")!=0 || strcmp(argv[3],"-b")!=0){
        std::cout << "wrong imput give ./vaccineMonitor -c citizenRecordsFile –b bloomSize" << '\n';
        return -6;

    }
    std::fstream checkfile(argv[2]);

    unsigned int bloom=int_checker(std::string(argv[4]));
    if(bloom<=0){
        std::cout << "invalid bloomsize given" << '\n';
        return -3;
    }
    int numOflines=0;
    std::string word;
    if(!checkfile){
        std::cout << "invalid file name given" << '\n';
        return -5;
    }
    std::fstream file;
    file.open(argv[2]);

    while(!file.eof()) {
    	getline(file, word);
    	numOflines ++;
    }
    file.close();

    if(numOflines>10010) numOflines= 10010;
    numOflines=int(3/2*numOflines);
    std::string id,name,surname,country,age,virus,ans,date,date1,date2,line;
    int datChanged,act;
    bool badFlag=false,exitFlag;
    int i=0;
    std::string filename =std::string(argv[2]);
    VirusHash* vh= new VirusHash(bloom);
    RecordHash* rh= new RecordHash(numOflines);
    Record* rec;
    Record* checkRec;
    std::istringstream* iss;
    PlainList* vacc;
    PlainList* nonVacc;
    CountryNode* opa;
    int int_id;
    bool ages,today;


    file.open(argv[2]);
    badFlag=false;
    while (file >> word){
        datChanged=0;
        if (word.length()<=4) { //word is id
            //eisagwgh prohgoumenhs
            if(surname!=""){
                rec=new Record(id,name,surname,country,age);
                checkRec=rh->addEntry(rec);
                if (checkRec!=NULL){ // null means inconsistent rec
                    vh->insertVirusInHash(virus,checkRec,date,false);
                    if(checkRec!=rec) delete rec; //consistent but dupblicate , so delete
                }else {
                    std::cout << "ERROR WITH CITIZEN "<< id << '\n';
                    delete rec;
                }

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
                datChanged=1;
            }else date="-1";

        }else{ //word is false date
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
                datChanged=1;
            }else date="-1";
        }

    }
    if(badFlag!=true){
        rec=new Record(id,name,surname,country,age);
        checkRec=rh->addEntry(rec);
        if (checkRec!=NULL){ // null means inconsistent rec
            vh->insertVirusInHash(virus,checkRec,date,false);
            if(checkRec!=rec) delete rec; //consistent but dupblicate , so delete
        }else delete rec;
    }else std::cout << "ERROR WITH CITIZEN "<< id << '\n';

    file.close();

    std::cout << "virus file read and stored" << '\n';


    exitFlag=false;
    while(!exitFlag){

        std::cout << "give stuff" << '\n';
        if (!std::getline(std::cin, line) || line.empty()) badFlag=true;
        iss= new std::istringstream(line);
        (*iss)>>word;
        if(word=="/vaccineStatusBloom") act=1;
        else if(word=="/vaccineStatus")  act=2;
        else if(word=="/populationStatus"){act=3; ages=false;}
        else if(word=="/popStatusByAge"){act=3; ages=true;}
        else if(word=="/insertCitizenRecord"){act=4; today=false;}
        else if(word=="/vaccinateNow"){act=4; today=true;}
        else if(word=="/list-nonVaccinated-Persons")act=5;
        else if(word=="/exit")act=6;
        else act=7;

    //badFlag sth do
        badFlag=true;
        switch(act) {
            case 1:

                if(!(*iss>>id)) break;
                if(!(*iss>>virus))break;
                if((*iss>>word) )break;
                vh->vaccineStatusBloom(id,virus);
                badFlag=false;
                break;
            case 2:
                if(!(*iss>>id)) break;

                int_id=int_checker(id);

                if(int_id==-1)break;

                if(!(*iss>>virus)) vh->vaccineStatusAll(int_id);
                else{
                    if(*iss>>word)break;

                    vh->vaccineStatus(int_id,virus);
                }

                badFlag=false;
                break;
            case 3:
                i=0;
                while (*iss >> word){
                    switch(i) {
                            case 0:
                                country=word;
                                break;
                            case 1:
                                virus=word;
                                break;
                            case 2:
                                date1=word;
                                break;
                            case 3:
                                date2=word;
                                break;
                            default:
                                break;
                        }
                        i++;
                }
                if (i==3 || i==4){
                    if(i==3){
                        date2=date1;
                        date1=virus;
                        virus=country;
                        country="all";
                    }
                }else break;
                vacc=vh->getLists(virus,true);
                nonVacc=vh->getLists(virus,false);
                opa= new CountryNode( country , true);
                if(dateValidor(date1)&&dateValidor(date2) && (vacc!=NULL || nonVacc!=NULL)){
                    opa->statsByCountry(date1,date2,vacc,nonVacc,ages);
                }else std::cout << "bad date given(2000-2021) or virus does not exist " << '\n';
                delete opa;
                badFlag=false;
                break;
            case 4:
                i=0;
                while (*iss >> word){
                    switch(i) {
                            case 0:
                                id=word;
                                break;
                            case 1:
                                name=word;
                                break;
                            case 2:
                                surname=word;
                                break;
                            case 3:
                                country=word;
                                break;
                            case 4:
                                age=word;
                                break;
                            case 5:
                                virus=word;
                                break;
                            case 6:
                                ans=word;
                                date="-1";
                                break;
                            case 7:
                                date=word;
                                break;
                            default:
                                break;
                        }
                        i++;
                }
                if (!((i==7 && ans=="NO")||(i==8 && ans=="YES" && dateValidor(date))||(i==6 && today))) break;
                if (int_checker(id)==-1 || int_checker(age)==-1) break;
                rec=new Record(id,name,surname,country,age);
                checkRec=rh->addEntry(rec);
                if (checkRec!=NULL){ // null means inconsistent rec
                    if(today) date=getTodaysDate();
                    vh->insertVirusInHash(virus,checkRec,date,true);
                    if(checkRec!=rec) delete rec; //consistent but dupblicate , so delete
                }else {
                    std::cout << "INCONSISTENT CITIZEN ID AND OTHER CHARACTERISTICS" << '\n';
                    delete rec;
                }
                badFlag=false;
                break;
            case 5:
                if(!(*iss>>virus)) break;
                if((*iss>>word) )break;
                nonVacc=vh->getLists(virus,false);
                std::cout << virus << '\n';
                if(nonVacc!=NULL) printPlain(nonVacc);
                else std::cout << "NO CITIZENS WITHOUT VACCINE ON "<< virus << '\n';
                deletePlain(nonVacc);
                badFlag=false;
                break;
            case 6:
                exitFlag=true;
                badFlag=false;
                break;
            case 7:
                break;
        }
        if(badFlag) std::cout << "INCORRECT DATA GIVEN" << '\n';
        delete iss;

    }
    delete vh;
    delete rh;

    return 0;

}
