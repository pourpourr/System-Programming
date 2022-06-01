#ifndef __HASH__
#define __HASH__

#include <string>
#include <iostream>



template <typename Value>
class List{
    private:
        class ListElement{     
            private:
                Value val;
                ListElement* next;      
            public:
                
                ListElement(Value v ){
                    this->val=v;
                    this->next=NULL;
                }
                friend class List;
        };
        ListElement* first;
    public:
      

        List(){
            this->first=NULL;
            
        }
        ~List(){
            if(this->first!=NULL){
                ListElement* Lptr=this->first;
                ListElement* temp=this->first->next;
                delete Lptr;
                while(temp!=NULL){
                    Lptr=temp;
                    temp=temp->next;
                    delete Lptr;
                }
                first=NULL;
            }
        }
        void addEntry(Value v){
            ListElement* Lptr=new ListElement(v);
           
            if(this->first!=NULL) Lptr->next=this->first;
 
            this->first=Lptr;
           
        }
        void ListConCat(List* l){
            if(this->first==NULL) {
                this->first=l->first;
                l->first=NULL;

                delete l;
            }else{
                ListElement* Lptr=this->first;
                while(Lptr->next!=NULL)Lptr=Lptr->next;
                Lptr->next=l->first; 
                l->first=NULL;

                delete l;
            }
         

        }
        void PrintList(){
            if(this->first!= NULL){
                ListElement* Lptr=this->first;
                std::cout<<"first is "<<Lptr->val<<"\n";
                while(Lptr->next!=NULL){
                    Lptr=Lptr->next;
                    std::cout<<"next is "<<Lptr->val<<"\n";
                }
            }else std::cout<<"list empty \n";
        }
        Value popFirst(){
            Value backup= this->first->val;
            ListElement* temp= this->first;
            this->first=this->first->next;
            delete temp;
            return backup;
            
        }
        bool isEmpty(){
            return this->first==NULL;
        }

        bool isInside(Value v){
            if(this->first==NULL)return false;
            ListElement* temp= this->first;
            while(temp!=NULL){
                if(temp->val==v)return true;
                temp=temp->next;
            }
            return false;
        }

};

template <typename Value>
class Node
{
    private:
        std::string key;
        Value* val;
        class Node* next;
    public:
        Node(){
            key="NULL";
            this->next=NULL;
            this->val=NULL;
        }
        ~Node(){   
            delete this->val;
            if(this->next!=NULL) delete this->next;
        }

        
        void insert(std::string k ,Value* v ){
            if(this->key=="NULL" ) {this->key=k; this->val=v; /*return v;*/}
            else if (this->key==k) {/*std::cout<< "key with value "<< k << " already inside"; return this->val;*/}            
            else{
                if(this->next==NULL)this->next = new Node;
                this->next->insert(k,v);            
            }
        }
        
        Value* search(std::string k){
            if(this->key=="NULL")return NULL; 
            if(this->key==k)return this->val;
            if(this->next!=NULL)return this->next->search(k);
            return NULL;
        }
        void PrintNode(){
            if(this->key!="NULL"){
                std::cout<< "key is "<<this->key<< " \n" ;
                if(this->next!=NULL)this->next->PrintNode();
            }
        }

        List<Value*>* asList(){
            if(this->key=="NULL") return NULL;
            List<Value*>* l= new List<Value*>;
            Node* temp=this;
            if(temp->next==NULL)l->addEntry(temp->val);
            else{
                while (temp!=NULL)
                {
                l->addEntry(temp->val);
                    temp=temp->next;
                    

                }
            }
            return l;
        }
        //friend class Hash;

};

template <typename Value>
class Hash
{
    private:
        class Node<Value>** nodes;
        unsigned int hashSize;
    public:
        Hash(unsigned int hS){
            this->nodes= new Node<Value>*[hS];
            for(int i=0; i<hS; i++){
                this->nodes[i]=new Node<Value>;
                
            }
            this->hashSize=hS;
        }
        ~Hash(){
            for(int i=0; i<this->hashSize; i++){
                delete this->nodes[i];  
   
            }
            delete[] this->nodes;
        }
        void PrintHash(){
            int i;
            for(i=0; i<this->hashSize; i++){
                (this->nodes[i])->PrintNode();
            }
        }
        void insertInHash(std::string key,Value* v ){
            
            unsigned int hashVal=djb2((unsigned char* )key.c_str());
            this->nodes[hashVal]->insert(key,v);
        }

        Value* searchInHash(std::string key){
            unsigned int hashVal=djb2((unsigned char* )key.c_str());
            return this->nodes[hashVal]->search(key);
        }
        unsigned int djb2(unsigned char* str) {
            unsigned long hash = 5381;
            int c;
            while (c = *str++) {
                hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
            }
            return hash%(this->hashSize);
        }
        List<Value*>* asList(){
            List<Value*>*l;

            l=new List<Value*>;

            List<Value*>* k;
            for(int i=0; i<this->hashSize; i++){
                k=(this->nodes[i])->asList();
                  

                if(k!=NULL)l->ListConCat(k);
            }

            return l;
        }
        

};






#endif
