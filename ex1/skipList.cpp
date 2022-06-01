#include "skipList.h"


skipList::skipList(){
    int i;
    this->lvlHeader=new SLnode*[MAX_LEVEL];
    for(i=0; i<MAX_LEVEL; i++)  lvlHeader[i]=NULL;
}

skipList::~skipList(){
    if(this->lvlHeader[0]!= NULL){
        SLnode* nodePtr= this->lvlHeader[0];
        SLnode* del= nodePtr->next[0];
        delete nodePtr;
        while(del!=NULL){
            nodePtr=del;
            del=del->next[0];
            delete nodePtr;
        }
    }
    delete[] this->lvlHeader;
}

SLnode* skipList::searchSL(int rId){

    int lvl=MAX_LEVEL-1;
    int i,posFound;
    // if posFound=0 item is inside || posFound=1 item will be inserted right next to backUp
    //posFound=2 means item will be the smallest of all items
    while(this->lvlHeader[lvl]==NULL){
        if(lvl>0) lvl--;
        else break;
    }
    SLnode* lvlPtr=lvlHeader[lvl]; // HIGHEST POINTER IN LIST
    if (lvlPtr==NULL){
        return NULL;
    }else{
        SLnode* backUp=lvlPtr; // last pos where rId> id .
        posFound=0;
        while(lvlPtr->id!= rId && posFound==0){
            if(lvlPtr->id>rId){
                if (lvl==0){ //cannot go any further down
                    if (lvlPtr!=this->lvlHeader[0]) lvlPtr=this->lvlHeader[0]; //start from the bigging of lvl 0
                    else posFound=2; //we are and lvl 0 start and our is  smaller so we exit
                    backUp=lvlPtr;
                }else{ // one level down
                    lvl--;
                    if (backUp==this->lvlHeader[lvl+1]){
                        lvlPtr=this->lvlHeader[lvl];
                        backUp=lvlPtr;
                    }else lvlPtr=backUp;
                }
            }else{
                backUp=lvlPtr;
                if (lvlPtr->next[lvl]==NULL){
                    if (lvl==0) posFound=1;
                    else lvl--;
                }else{
                    if ((lvlPtr->next[lvl])->id>rId && lvl==0) posFound=1;
                    else lvlPtr=lvlPtr->next[lvl];
                }
            }
        }//while
        if(posFound==0) return lvlPtr;
        else return NULL;
    }
}


void skipList::deleteSL(int rId){
    int lvl=MAX_LEVEL-1;
    int i;

    while(this->lvlHeader[lvl]==NULL){
        if(lvl>0) lvl--;
        else break;
    }

    SLnode* lvlPtr=lvlHeader[lvl]; // HIGHEST POINTER IN LIST
    if (lvlPtr!=NULL){
        lvlPtr=searchSL(rId);
        if(lvlPtr!=NULL){
            SLnode* temp;
            for(i=0; i<lvlPtr->level; i++){
                if(lvlPtr!= this->lvlHeader[i]){
                    temp=lvlPtr->prev[i];
                    temp->next[i]=lvlPtr->next[i];
                    if (lvlPtr->next[i]!=NULL) (lvlPtr->next[i])->prev[i]=temp;
                }else{
                    temp=lvlPtr->next[i];
                    this->lvlHeader[i]=temp;
                    if (temp!=NULL) temp->prev[i]=temp;
                }
            }//for
            delete lvlPtr;
        }//item not inside
    }//lvlPtr!=NULL skipList is null
}


void skipList::PrintSlist(){
    for(int i=MAX_LEVEL-1; i>=0; i--){
        if(this->lvlHeader[i]!= NULL){
            std::cout << "level "<< i << '\n';
            SLnode* nodePtr= this->lvlHeader[i];
            std::cout<<"first node = " << nodePtr->id <<" date ==" << nodePtr->date<< "||---";
            while(nodePtr->next[i]!=NULL){
                nodePtr=nodePtr->next[i];
                std::cout<<"|| " << nodePtr->id <<" date ==" << nodePtr->date<<"||---";
            }
            std::cout  << '\n';
        }
    }
}

void skipList::insertSL(int rId,Record* r,std::string str){
    //search correct position
    int lvl=MAX_LEVEL-1;
    int i,posFound;

    while(this->lvlHeader[lvl]==NULL){
        if(lvl>0) lvl--;
        else break;
    }
    SLnode* lvlPtr=lvlHeader[lvl]; // HIGHEST POINTER IN LIST
    if (lvlPtr==NULL){
        SLnode* temp= new SLnode(rId, r ,str, randomlvl());
        for (i=0; i<temp->level; i++){
            this->lvlHeader[i]=temp;
            temp->next[i]=NULL;
            temp->prev[i]=temp;

        }
    }else{
        SLnode* backUp=lvlPtr; // last pos where rId
        posFound=0;
        while(lvlPtr->id!= rId && posFound==0){
            if(lvlPtr->id>rId){
                if (lvl==0){
                    if (lvlPtr!=this->lvlHeader[0]) lvlPtr=this->lvlHeader[0];
                    else posFound=2;
                    backUp=lvlPtr;
                }else{
                    lvl--;
                    if (backUp==this->lvlHeader[lvl+1]){
                        lvlPtr=this->lvlHeader[lvl];
                        backUp=lvlPtr;
                    }else lvlPtr=backUp;
                }
            }else{ //lvlPtr->id<rId
                backUp=lvlPtr;
                if (lvlPtr->next[lvl]==NULL){
                    if (lvl==0) posFound=1;
                    else lvl--;
                }else{
                    if ((lvlPtr->next[lvl])->id > rId && lvl==0) posFound=1;
                    else lvlPtr=lvlPtr->next[lvl];
                }
            }
        }//while
        if(posFound!=0){
            SLnode* temp= new SLnode(rId , r,str, randomlvl());
            int flag=0;
            if(posFound==1){
                temp->next[0]=backUp->next[0];
                if (backUp->next[0]!=NULL) (backUp->next[0])->prev[0]=temp;
                backUp->next[0]=temp;
                temp->prev[0]=backUp;
                for(i=1; i<temp->level; i++){
                    while(backUp->level<=i){
                        if(backUp->level==i){
                            if (backUp->prev[i-1]!=backUp) backUp=backUp->prev[i-1];
                            else flag=-1;
                        }else{
                            if (backUp->prev[i]!=backUp) backUp=backUp->prev[i];
                            else flag=-1;
                        }
                        if (flag==-1) break;
                    }

                    if (flag==-1) break;
                    temp->next[i]=backUp->next[i];
                    if (backUp->next[i]!=NULL)(backUp->next[i])->prev[i]=temp;
                    backUp->next[i]=temp;
                    temp->prev[i]=backUp;
                }
                if (i<temp->level){
                    for(i=i; i<temp->level; i++){
                        if (this->lvlHeader[i]==NULL){
                            this->lvlHeader[i]=temp;
                            temp->next[i]=NULL;
                            temp->prev[i]=temp;
                        }else{
                            temp->next[i]=this->lvlHeader[i];
                            (this->lvlHeader[i])->prev[i]=temp;
                            this->lvlHeader[i]=temp;
                            temp->prev[i]=temp;
                        }
                    }
                }
            }else{//posFound=2
                temp->next[0]=backUp;
                temp->prev[0]=temp;
                backUp->prev[0]=temp;
                this->lvlHeader[0]=temp;
                for(i=1; i<temp->level; i++){ //if level header on level 0 then for sure on all the other levels
                    temp->prev[i]=temp;
                    temp->next[i]=this->lvlHeader[i];
                    if(this->lvlHeader[i]!=NULL) (this->lvlHeader[i])->prev[i]=temp;
                    this->lvlHeader[i]=temp;
                }//for
            }//else posFound=2
        }//item Already inside
    }//else lvlPtr!=NULL

}

PlainList* skipList::asPlain(){
    PlainList* listHeader=NULL;
    SLnode* nodePtr= this->lvlHeader[0];
    while (nodePtr!=NULL) {
        insertPlain(&listHeader,nodePtr->rec,nodePtr->date);
        nodePtr=nodePtr->next[0];
    }
    return listHeader;
}

void insertPlain(PlainList** first, Record* r , std::string d){
    if((*first)==NULL){
        *first= new PlainList;
        (*first)->date=d;
        (*first)->rec=r;
        (*first)->next=NULL;
    }else{
        PlainList* nodePtr= new PlainList;
        nodePtr->next=(*first);
        nodePtr->rec=r;
        nodePtr->date=d;
        (*first)=nodePtr;
    }
}

void printPlain(PlainList* first){
    if(first!=NULL){
        PlainList* nodePtr= first;
        while(nodePtr!=NULL){
            nodePtr->rec->printRec();
            nodePtr=nodePtr->next;
        }
    }
}

void deletePlain(PlainList* first){
    if(first!=NULL){
        PlainList* nodePtr= first;
        PlainList* temp= nodePtr->next;
        delete nodePtr;
        while(temp!=NULL){
            nodePtr=temp;
            temp=temp->next;
            delete nodePtr;
        }
    }
}

PlainList* popFirst(PlainList** first){
    if((*first)!=NULL){
        PlainList* temp= *first;
        *first= (*first)->next;
        temp->next=NULL;
        return temp;
    }
    return NULL;
}


SLnode::SLnode(int rId,Record* r,std::string str , int lvl){
    this->id=rId;
    this->level=lvl;
    this->rec=r;
    this->date=str;
    this->next=new SLnode*[this->level];
    this->prev=new SLnode*[this->level];
    int i;
    for(i=0; i<this->level; i++){
        this->next[i]=NULL;
        this->prev[i]=NULL;
    }
}

SLnode::~SLnode(){
    delete[] this->next;
    delete[] this->prev;
}

int randomlvl(){
    int level=1;//sgr 1
    while(level<MAX_LEVEL && (rand()%101)<P)level++;
    return level;
}
