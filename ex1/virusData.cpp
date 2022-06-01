#include "virusData.h"

VirusHash::VirusHash(unsigned int bloom){
    int i;
    for(i=0; i<NUM_OF_VIRUSES; i++){
        this->viruses[i]=new VirusNode(bloom);
    }
}

VirusHash::~VirusHash(){
    int i;
    for(i=0; i<NUM_OF_VIRUSES; i++){
        delete this->viruses[i];
    }
}

void VirusHash::insertVirusInHash(std::string vir ,Record* rec , std::string date , bool manual){

    int key=djb2((unsigned char*)vir.c_str())%NUM_OF_VIRUSES;
    if(!manual)(this->viruses[key])->insertVirus(vir,rec,date);
    else (this->viruses[key])->insertVirusManual(vir,rec,date);
}

void VirusHash::PrintVHash(){
    int i;
    for(i=0; i<NUM_OF_VIRUSES; i++){
        (this->viruses[i])->PrintVNode();
    }
}

VirusNode::VirusNode(int bloom){
    this->virus="NULL";
    this->next=NULL;
    this->vaccinated=new skipList;
    this->nonVaccinated=new skipList;
    this->vaccBloom=new BloomFilter(bloom);
    this->bloomsize=bloom;



}

VirusNode::~VirusNode(){
    delete this->vaccinated;
    delete this->nonVaccinated;
    delete this->vaccBloom;
    delete this->next;

}

void VirusNode::insertVirus(std::string vir , Record* rec , std::string date ){
    if(this->virus=="NULL"||this->virus==vir) {
        this->virus=vir;
        if((((this->vaccinated)->searchSL(rec->getId()))==NULL)&&((this->nonVaccinated)->searchSL(rec->getId())==NULL)) {
            if(date=="-1") (this->nonVaccinated)->insertSL(rec->getId(),rec,date);
            else {
                char id[4];
                sprintf(id, "%d", rec->getId());
                (this->vaccBloom)->wordInsert(id);
                (this->vaccinated)->insertSL(rec->getId(),rec,date);

            }
        }else std::cout << "ERROR: CITIZEN "<<rec->getId()<<" ALREADY INSERTED IN "<< virus<<" DATABASE" << '\n';
        //person already in SL cannot insert again. No need to delete rec because , is consistent record so it will deleted after
    }else{
        if(this->next==NULL) this->next=new VirusNode(this->bloomsize); // if next does not exist create it
        (this->next)->insertVirus(vir, rec , date);
    }
}


void VirusNode::insertVirusManual(std::string vir , Record* rec , std::string date ){
    if(this->virus=="NULL"||this->virus==vir) {
        this->virus=vir;
        SLnode* vaccSearch=(this->vaccinated)->searchSL(rec->getId());
        if(vaccSearch==NULL){
            if((this->nonVaccinated)->searchSL(rec->getId())==NULL){
                if(date=="-1") (this->nonVaccinated)->insertSL(rec->getId(),rec,date);
                else {
                    char id[4];
                    sprintf(id, "%d", rec->getId());
                    (this->vaccBloom)->wordInsert(id);
                    (this->vaccinated)->insertSL(rec->getId(),rec,date);
                }
            }else if (date!="-1"){ //he is not vaccinated but no we have a vaccine date
                (this->nonVaccinated)->deleteSL(rec->getId());
                char id[4];
                sprintf(id, "%d", rec->getId());
                (this->vaccBloom)->wordInsert(id);
                (this->vaccinated)->insertSL(rec->getId(),rec,date);
            }
        }else std::cout << "ERROR: CITIZEN  "<<rec->getId()<<" ALREADY VACCINATED ON "<< vaccSearch->getDate() << '\n';

    }else{
        if(this->next==NULL) this->next=new VirusNode(this->bloomsize); // if next does not exist create it
        (this->next)->insertVirusManual(vir, rec , date);
    }

}

void VirusNode::PrintVNode(){
    if(this->virus!="NULL"){
        std::cout << "virus = " <<this->virus<<  '\n';
        std::cout << "----vaccinated----" << '\n';
        (this->vaccinated)->PrintSlist();
        std::cout << "----nonVaccinated----" << '\n';
        (this->nonVaccinated)->PrintSlist();
        if(this->next!=NULL) {
            std::cout << "next virus=" << '\n';
            (this->next)->PrintVNode();
        }else std::cout << "===================" << '\n';
    }
}

PlainList*  VirusHash::getLists(std::string vir, bool vacc){

    int key=djb2((unsigned char*)vir.c_str())%NUM_OF_VIRUSES;
    bool flag=false;
    VirusNode* nodePtr=this->viruses[key];
    if(!(nodePtr->virus=="NULL")){
        while(nodePtr!=NULL && !flag){
            if (nodePtr->virus == vir) flag=true;
            else nodePtr=nodePtr->next;
        }
        if(nodePtr!=NULL){
            if(vacc) return (nodePtr->vaccinated)->asPlain();
            else return (nodePtr->nonVaccinated)->asPlain();
        }
    }
    else std::cout << "NO CITIZENS RELATED TO VIRUS "<< vir << '\n';
    return NULL;
}

void VirusHash::vaccineStatusBloom(std::string id, std::string vir){
    int key=djb2((unsigned char*)vir.c_str())%NUM_OF_VIRUSES;

    bool flag=false;
    VirusNode* nodePtr=this->viruses[key];
    if(!(nodePtr->virus=="NULL")){
        while(nodePtr!=NULL && !flag){
            if (nodePtr->virus == vir) flag=true;
            else nodePtr=nodePtr->next;
        }
        if(nodePtr!=NULL){
            int result=(nodePtr->vaccBloom)->wordSearch((char *)id.c_str());
            if(result==0) std::cout << "NOT VACCINATED" << '\n';
            else std::cout << "MAYBE" << '\n';
        }else std::cout << "NOT VACCINATED" << '\n';

    }else std::cout << "NOT VACCINATED" << '\n';



}

void VirusHash::vaccineStatus(int id ,std::string vir){
    int key=djb2((unsigned char*)vir.c_str())%NUM_OF_VIRUSES;
    bool flag=false;
    VirusNode* nodePtr=this->viruses[key];
    if(!(nodePtr->virus=="NULL")){
        while(nodePtr!=NULL && !flag){
            if (nodePtr->virus == vir) {
                flag=true;
                break;
            }
            nodePtr=nodePtr->next;
        }
        if(flag){
            SLnode* node_sl= (nodePtr->vaccinated)->searchSL(id);
            if (node_sl==NULL) std::cout << "NOT VACCINATED" << '\n';
            else std::cout << "vaccinated on "<< node_sl->getDate()<< '\n';
        }else std::cout << "NOT VACCINATED" << '\n';
    }else std::cout << "NOT VACCINATED" << '\n';

}

void VirusHash::vaccineStatusAll(int id){
    int i;
    VirusNode* nodePtr;
    SLnode* node_sl;
    for(i=0; i<NUM_OF_VIRUSES; i++){
        if(!((this->viruses[i])->virus=="NULL")){
            nodePtr=this->viruses[i];
            while (nodePtr!=NULL) {
                node_sl= (nodePtr->vaccinated)->searchSL(id);
                if(node_sl==NULL){
                    node_sl= (nodePtr->nonVaccinated)->searchSL(id);
                    if(node_sl!=NULL) std::cout << nodePtr->virus<<" NO" << '\n';
                }else std::cout << nodePtr->virus<<" YES ON "<< node_sl->getDate() << '\n';
                nodePtr=nodePtr->next;
            }//while
        }//if
    }//for
}
