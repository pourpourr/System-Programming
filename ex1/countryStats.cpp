#include "countryStats.h"

CountryNode::CountryNode(std::string name, bool ages){
    this->name=name;
    this->next=NULL;
    this->general=0;
    this->vaccinated=0;
    if(ages){
        this->agesYes= new double[4];
        for(int i=0; i<4; i++) this->agesYes[i]=0;
    }else this->agesYes=NULL;
}

CountryNode::~CountryNode(){
    if(this->agesYes!=NULL)  delete[] this->agesYes;
    if(this->next!=NULL) delete this->next;
}

void CountryNode::PrintCNode(){
    if(this->name!="NULL"){
        std::cout << "country = " <<this->name;
        std::cout << " ||general= "<<this->general << '\n';
        if(this->next!=NULL) {
            std::cout << "next country =" << '\n';
            (this->next)->PrintCNode();
        }else std::cout << "===================" << '\n';
    }
}

void CountryNode::statsByCountry(std::string date1, std::string date2 , PlainList* vaccList , PlainList* nonVaccList,  bool ages){
    std::string name=this->name;
    if(name!="all"){
        PlainList* list= popFirst(&vaccList);
        int i,age;
        CountryNode* nodePtr= new CountryNode(name,ages);
        while (list!=NULL) {
            if((list->rec)->getCountry()==name){
                nodePtr->general++;
                if (dateChecker(date1,date2,list->date)){
                    nodePtr->vaccinated++;
                    if(ages){
                        age=(list->rec)->getAge();
                        if(age<=60) nodePtr->agesYes[age/20]++;
                        else nodePtr->agesYes[3]++;
                    }
                }
            }
            deletePlain(list);
            list=popFirst(&vaccList);
        }
        list=popFirst(&nonVaccList);
        while (list!=NULL) {
            if((list->rec)->getCountry()==name) nodePtr->general++;
            deletePlain(list);
            list=popFirst(&nonVaccList);
        }
        if(nodePtr->general!=0){
            if(ages){
                std::cout << name << '\n';
                std::cout << "0-20 "<<nodePtr->agesYes[0]<<" "<<(nodePtr->agesYes[0] / nodePtr->general)*100<< "%"  << '\n';
                std::cout << "20-40 "<<nodePtr->agesYes[1]<<" "<< (nodePtr->agesYes[1] / nodePtr->general)*100<< "%"  << '\n';
                std::cout << "40-60 "<<nodePtr->agesYes[2]<<" "<< (nodePtr->agesYes[2] / nodePtr->general)*100<< "%"  << '\n';
                std::cout << "60+ "<<nodePtr->agesYes[3]<<" "<<(nodePtr->agesYes[3] / nodePtr->general)*100<< "%"  << '\n';
            }else std::cout << name<<" " <<nodePtr->vaccinated<<" "<<( nodePtr->vaccinated / nodePtr->general)*100<< "%" << '\n';
        }else std::cout << "nobody  this virus in "<< name << '\n';
        delete nodePtr;
    }else{

        bool found=false;
        int i,age;
        CountryNode* first= new CountryNode("NULL",ages);
        CountryNode* nodePtr;
        PlainList* list= popFirst(&vaccList);
        while(list!=NULL){
            nodePtr=searchCountry(first,(list->rec)->getCountry(), ages );
            nodePtr->general++;
            if(dateChecker(date1,date2,list->date)){
                nodePtr->vaccinated++;
                if(ages){
                    age=(list->rec)->getAge();
                    if(age<60) nodePtr->agesYes[age/20]++;
                    else nodePtr->agesYes[3]++;
                }
            }
            deletePlain(list);
            list=popFirst(&vaccList);
        }
        list=popFirst(&nonVaccList);
        while (list!=NULL) {
            nodePtr=searchCountry(first,(list->rec)->getCountry(), ages );
            nodePtr->general++;
            deletePlain(list);
            list=popFirst(&nonVaccList);
        }
        nodePtr=first;
        while(nodePtr!=NULL){
            if(ages){
                std::cout << nodePtr->name << '\n';
                std::cout << "0-20 "<<nodePtr->agesYes[0]<<" "<<(nodePtr->agesYes[0] / nodePtr->general)*100<< "%"  << '\n';
                std::cout << "20-40 "<<nodePtr->agesYes[1]<<" "<< (nodePtr->agesYes[1] / nodePtr->general)*100<< "%"  << '\n';
                std::cout << "40-60 "<<nodePtr->agesYes[2]<<" "<< (nodePtr->agesYes[2] / nodePtr->general)*100<< "%"  << '\n';
                std::cout << "60+ "<<nodePtr->agesYes[3]<<" "<<(nodePtr->agesYes[3] / nodePtr->general)*100<< "%"  << '\n';
            }else std::cout << nodePtr->name<<" " <<nodePtr->vaccinated<<" "<<( nodePtr->vaccinated / nodePtr->general)*100<< "%" << '\n';
            nodePtr=nodePtr->next;
        }
        delete first;
    }
}

CountryNode* CountryNode::searchCountry(CountryNode* first, std::string name , bool ages){
//searchCountry returns the country or makes a new countrynode and returns it
    if(first->name=="NULL" || first->name==name){
        first->name=name;
        return first;
    }
    CountryNode* nodePtr= first;
    bool found= false;
    while (found==false && nodePtr!=NULL){
        if(nodePtr->name==name) found=true;
        else nodePtr=nodePtr->next;
    }
    if(found) return nodePtr;
    nodePtr= new CountryNode(name, ages);
    nodePtr->next=first->next;
    first->next= nodePtr;
    return nodePtr;

}
