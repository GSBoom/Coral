#ifndef CORAL_H
#define CORAL_H



#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <stdlib.h>
#include <bitset>
#include <memory.h>
#include <sys/time.h>
#include "hashfunctions.h"
#include <cstdint>
#include "basketused.h"
#include "BloomFilter.h"
#include "HotDefine.h"


using namespace std;
#define prime 739
#define bigger_p 1048576
#define timer 5
#define M 80000

#define Roomnum 16// This is the parameter to controll the maximum number of rooms in a bucket. 
#define MatrixWidth 4//this is the paremeter of basket
#define Increasefingerpoint 2//this means 2^Increasefingerpoint=MatrixWidth

struct Cellloacl{
    uint16_t src;
    uint16_t dst;
    int weight;
    uint8_t index; 
};

struct Cellglobal{
    uint16_t src;
    uint16_t dst;
    int weight;
    uint8_t index;
    uint32_t basketidx;
};

struct infoincell{
    uint32_t index;
    
    uint8_t cellindex;
    uint16_t fingerpoint;
};

struct basket {
	Cellloacl lmatrix[MatrixWidth*MatrixWidth];
    Cellglobal gmatrix[MatrixWidth*MatrixWidth];
    bool isback;//Is the global area still open to the public or has it been taken back?
    vector<int> next;//f this bucket is full, the data will be stored in the next basket.
    basket(){
        isback=false;//By default, it is open to the public. It will only be taken back when one needs to use it.
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            memset(lmatrix, 0, sizeof(lmatrix));
            memset(gmatrix, 0, sizeof(gmatrix));
        }
        
    }

};




class Coral {
private:
	int width;						/* the width of matrix */
	int fingerprint_length;			/* finger print length */
    
    vector<basketused> availbasket; /*information of baskets used*/
    vector<int> oralused;
    vector<int> kinds;
    map<int,vector<int>> match; /* hot row match cold row*/ 
    vector<uint32_t> rowsuse;
    

public:
    basket* value;                 /*basket matrix*/
    int widthpublic;
	map<unsigned int, int> index;
    HotIdentify hotrows;
	int n;
    //构造函数
    Coral(int width, int fingerprint_length,int maxhotrownumber,double precisionrate,int comparenumber,int hotrowtimes):
        width(width),fingerprint_length(fingerprint_length),widthpublic(width),hotrows(maxhotrownumber,precisionrate,comparenumber,hotrowtimes) {
            n = 0;
            value = new basket[width * width];
            memset(value, 0, sizeof(basket) * width * width);
            for(int i=0;i<width*width;i++){
                basketused a;
                availbasket.push_back(a);
                oralused.push_back(0);
                kinds.push_back(0);
            }

            for(int j=0;j<width;j++){
                rowsuse.push_back(0);    
            }
            

    }

    ~Coral() {
		delete[] value;
	}

    /* Function: It is designed to be parallel */
   
    int findempty();//basket of position k is full,search a proper basket
    int findempty(int k);//basket of position k is full,search a proper basket
    bool getcoldrowstrict(int row,int column,int &newrow,double &alltime);//basket of position (row,column) is full,search a proper basket
    bool getcoldrow(int row,int column,int &newrow,double &alltime);
    bool getcoldrowstrict_fixed(int row,int column,int &newrow,double &alltime);
    bool getcoldrow_fixed(int row,int column,int &newrow,double &alltime);
    
    /* Function to print the internal situation of Coral */
    void printused();
    void printoralused(string path);
    void printavailable(string path);
    void printlocaluse(string path);
    void printglobaluse(string path);
    void printkinds(string path);
    void printpotentialhot(string path);
    void printmatch(string path);
    void printbasicparameter();

    /* Operation function to display various functions of Coral */
    bool freshedgestore(uint32_t bucketpos,int cellpos,uint16_t g1,uint16_t g2,int weight,double &alltime );
    bool freshedgestorec(uint32_t bucketpos,int cellpos,uint16_t g1,uint16_t g2,int weight,double &alltime );
	bool insert(string s1, string s2,int weight);
    bool insertc(string s1, string s2,int weight);
    double insertTime(string s1, string s2, int weight);
	int edgeWeightQuery(string s1, string s2);  		// s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
	int nodeWeightQuery(string s1, int type); 			//src_type = 0 dst_type = 1
    int nodeDegreeQuery(string s1,int type);           //src_type = 0 dst_type = 1
    int nodeDegreeQuery(infoincell s1,int type);       //src_type = 0 dst_type = 1
    bool giveback(int pos,double &alltime );
    bool givebackc(int pos,double &alltime );
	bool reachabilityQuery(string s1, string s2);
    void findnextnode(string s1,vector<infoincell> &nodes);
    void findnextnode(infoincell s1,vector<infoincell> &nodes);
    int findnearbyvertex(string s1,int k);
    void TopKheavyhitter(vector<string> &lotsnode);
	
};


// If all rows are determined to be hot rows, this mode of searching one by one has to be adopted.


int Coral:: findempty(int k){
    int res=0;
    basketused minnum=availbasket[0];
    if(k%2==0){

    for (int i = 0; i < availbasket.size(); i++){
        if(availbasket[i].localuse==MatrixWidth*MatrixWidth) continue;
        if(availbasket[i].globaluse==0) return i;
        if(availbasket[i]<minnum){
            res=i;
            minnum=availbasket[i];
        }
    }
    if(minnum.globaluse==MatrixWidth*MatrixWidth)cout<<"no spare space,insert fail"<<endl;
    return res;
    }

    else{
        for (int i = 0; i < availbasket.size(); i++){
            int pos=width*width-i-1;
            if(availbasket[pos].localuse==MatrixWidth*MatrixWidth) continue;
            if(availbasket[pos].globaluse==0) return pos;
            if(availbasket[pos]<minnum){
            res=pos;
            minnum=availbasket[pos];
            }
        }

    if(minnum.globaluse==MatrixWidth*MatrixWidth)cout<<"no spare space,insert failed"<<endl;
    
    return res;
    }
    return res;
}

int Coral:: findempty(){
    int from=generateRandomNumber(width*width-1);
    for(int i=0;i<width*width;i++){
        if(availbasket[(from+i)%(width*width)].globaluse<(MatrixWidth*MatrixWidth)&&value[(from+i)%(width*width)].isback==false){
            return (from+i)%(width*width);
        }
    }
    cout<<"no more place, need new Coral"<<endl;
    return -1;
}




bool Coral:: getcoldrowstrict(int row,int column,int &newrow,double &alltime){
    timeval ts,te;
    
    gettimeofday(&ts,NULL);
    
    int startrowid=generateRandomNumber(width-1);
    for(int i=0;i<width;i++){
        int rownum=(startrowid+i)%width;
        if(!hotrows.bloomfilter.mayContain(to_string(rownum))&&(hotrows.potentialhotrow.count(rownum)<=0)){
            match[row].push_back(rownum);
            hotrows.potentialhotrow[rownum]=1;
            newrow=rownum;
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            return true;
        }
    }
    gettimeofday(&te,NULL);
    alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    return false;
}


bool Coral:: getcoldrow(int row,int column,int &newrow,double &alltime){
    timeval ts,te;
    gettimeofday(&ts,NULL);
    int startrowid=generateRandomNumber(width-1);
    for(int i=0;i<width;i++){
        int rownum=(startrowid+i)%width;
        if(!hotrows.bloomfilter.mayContain(to_string(rownum))){
            match[row].push_back(rownum);
            newrow=rownum;
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            return true;
        }
    }
    gettimeofday(&te,NULL);
    alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    return false;
}



bool Coral::getcoldrowstrict_fixed(int row,int column,int &newrow,double &alltime){
    return false;
}

bool Coral::getcoldrow_fixed(int row,int column,int &newrow,double &alltime){
    int startrowid=(row+1)%width;
    for(int i=0;i<width;i++){
        if(rowsuse[startrowid+i]<=(1.5*width*MatrixWidth*MatrixWidth)) 
        {
        newrow=startrowid+i;
        match[row].push_back(newrow);
        return true;} 
    }
    return false;


}



bool Coral:: freshedgestore(uint32_t bucketpos,int cellpos,uint16_t g1,uint16_t g2,int weight,double &alltime){//Insert a new edge
    
    int insertnowrow=match[bucketpos/width][match[bucketpos/width].size()-1];
    int insertnocolumn=bucketpos%width;


    int a=value[bucketpos].next.size();
    int lastbucketpos=value[bucketpos].next[a-1];
    
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[lastbucketpos].gmatrix[cellpos].src==0){
            value[lastbucketpos].gmatrix[cellpos].src=g1;
            value[lastbucketpos].gmatrix[cellpos].dst=g2;
            value[lastbucketpos].gmatrix[cellpos].weight=weight;
            value[lastbucketpos].gmatrix[cellpos].index=i;
            value[lastbucketpos].gmatrix[cellpos].basketidx=bucketpos;
            availbasket[lastbucketpos].globaluse++;
            rowsuse[lastbucketpos/width]++;
           
            return true;
        }
        cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
    }



    
    for(int j=0;j<width;j++){
        if(value[insertnowrow*width+insertnocolumn].isback==true||availbasket[insertnowrow*width+insertnocolumn].globaluse>=MatrixWidth*MatrixWidth){
            insertnocolumn=(insertnocolumn+1)%width;
            continue;
        }
        for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                
            if(value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].src==0){
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].src=g1;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].dst=g2;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].weight=weight;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].index=k;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].basketidx=bucketpos;
                availbasket[insertnowrow*width+insertnocolumn].globaluse++;
                rowsuse[insertnowrow]++;
                value[bucketpos].next.push_back(insertnowrow*width+insertnocolumn);
                return true;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }
        insertnocolumn=(insertnocolumn+1)%width;
    }

    //find a new cold row
    int newcoldrow;
    
    if(getcoldrowstrict(bucketpos/width,bucketpos%width,newcoldrow,alltime)){
        for(int j=0;j<width;j++){
            if(value[newcoldrow*width+insertnocolumn].isback==true) {
                insertnocolumn=(insertnocolumn+1)%width;
                continue;
            }
            if(availbasket[newcoldrow*width+insertnocolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                    if(value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src==0&&value[newcoldrow*width+insertnocolumn].isback==false){
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src=g1;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].dst=g2;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].index=k;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].basketidx=bucketpos;
                        availbasket[newcoldrow*width+insertnocolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        value[bucketpos].next.push_back(newcoldrow*width+insertnocolumn);
                        return true;
                    }
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
                insertnocolumn=(insertnocolumn+1)%width;
            }
        }
    }

   

    if(getcoldrow(bucketpos/width,bucketpos%width,newcoldrow,alltime)){
        for(int j=0;j<width;j++){
            if(value[newcoldrow*width+insertnocolumn].isback==true) {
                insertnocolumn=(insertnocolumn+1)%width;
                continue;
            }
            if(availbasket[newcoldrow*width+insertnocolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                    if(value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src==0&&value[newcoldrow*width+insertnocolumn].isback==false){
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src=g1;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].dst=g2;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].index=k;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].basketidx=bucketpos;
                        availbasket[newcoldrow*width+insertnocolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        value[bucketpos].next.push_back(newcoldrow*width+insertnocolumn);
                        return true;
                    }
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            insertnocolumn=(insertnocolumn+1)%width;
        }
    }
        
    //all rows is hot
    int newbucketpos=findempty();
    if(newbucketpos==-1){
        cout<<" return bucket failed"<<endl;
        return false;
    }
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[newbucketpos].gmatrix[cellpos].src==0){
            value[newbucketpos].gmatrix[cellpos].src=g1;
            value[newbucketpos].gmatrix[cellpos].dst=g2;
            value[newbucketpos].gmatrix[cellpos].weight=weight;
            value[newbucketpos].gmatrix[cellpos].index=i;
            value[newbucketpos].gmatrix[cellpos].basketidx=bucketpos;
            availbasket[newbucketpos].globaluse++;
            rowsuse[newbucketpos/width]++;
            value[bucketpos].next.push_back(newbucketpos);
            return true;
        }
        cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
    }
    //if not inserted all positions had been used 
    return false;
}

//Debug version
bool Coral:: freshedgestorec(uint32_t bucketpos,int cellpos,uint16_t g1,uint16_t g2,int weight,double &alltime){
    
    
    
    cout<<"now size of over-flow "<<bucketpos<<" bucket "<<value[bucketpos].next.size();
    int insertnowrow=match[bucketpos/width][match[bucketpos/width].size()-1];
    int insertnocolumn=bucketpos%width;
    cout<<"had match cold row is "<<insertnowrow<<endl;
    cout<<"now size of over-flow "<<bucketpos<<" bucket "<<value[bucketpos].next.size()<<" ";
    int a=value[bucketpos].next.size();
    int lastbucketpos=value[bucketpos].next[a-1];
    cout<<"lastbucketpos :"<<lastbucketpos<<endl;
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[lastbucketpos].gmatrix[cellpos].src==0){            
            cout<<"g1 :"<<g1<<endl;
            value[lastbucketpos].gmatrix[cellpos].src=g1;           
            cout<<"g2 :"<<g2<<endl;
            value[lastbucketpos].gmatrix[cellpos].dst=g2;
            value[lastbucketpos].gmatrix[cellpos].weight=weight;
            value[lastbucketpos].gmatrix[cellpos].index=i;
            value[lastbucketpos].gmatrix[cellpos].basketidx=bucketpos;
            availbasket[lastbucketpos].globaluse++;
            rowsuse[lastbucketpos/width]++;
            cout<<"had a bucket spare,now size of over-flow "<<bucketpos<<" bucket is "<<value[bucketpos].next.size()<<endl;
            return true;
        }
        cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
    }

   
    for(int j=0;j<width;j++){
        if(value[insertnowrow*width+insertnocolumn].isback==true||availbasket[insertnowrow*width+insertnocolumn].globaluse>=MatrixWidth*MatrixWidth){
            insertnocolumn=(insertnocolumn+1)%width;
            continue;
        }
        for(int k=0;k<MatrixWidth*MatrixWidth;k++){
            cout<<"row "<<insertnowrow<<"column "<<insertnocolumn<<" has spare cell "<<endl;    
            if(value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].src==0){
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].src=g1;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].dst=g2;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].weight=weight;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].index=k;
                value[insertnowrow*width+insertnocolumn].gmatrix[cellpos].basketidx=bucketpos;
                availbasket[insertnowrow*width+insertnocolumn].globaluse++;
                rowsuse[insertnowrow]++;
                value[bucketpos].next.push_back(insertnowrow*width+insertnocolumn);
                cout<<"now size of over-flow "<<bucketpos<<" bucket is "<<value[bucketpos].next.size()<<endl;
                return true;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }
        insertnocolumn=(insertnocolumn+1)%width;
    }

   
    cout<<"now should be match a new cold row, now size of over-flow bucket"<<value[bucketpos].next.size();
    int newcoldrow;
    
    if(getcoldrowstrict_fixed(bucketpos/width,bucketpos%width,newcoldrow,alltime)){
        for(int j=0;j<width;j++){
            if(value[newcoldrow*width+insertnocolumn].isback==true) {
                insertnocolumn=(insertnocolumn+1)%width;
                continue;
            }
            if(availbasket[newcoldrow*width+insertnocolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                    if(value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src==0&&value[newcoldrow*width+insertnocolumn].isback==false){
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src=g1;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].dst=g2;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].index=k;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].basketidx=bucketpos;
                        availbasket[newcoldrow*width+insertnocolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        value[bucketpos].next.push_back(newcoldrow*width+insertnocolumn);
                        return true;
                    }
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
                insertnocolumn=(insertnocolumn+1)%width;
            }
        }
    }

    

    if(getcoldrow_fixed(bucketpos/width,bucketpos%width,newcoldrow,alltime)){
        for(int j=0;j<width;j++){
            if(value[newcoldrow*width+insertnocolumn].isback==true) {
                insertnocolumn=(insertnocolumn+1)%width;
                continue;
            }
            if(availbasket[newcoldrow*width+insertnocolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                    if(value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src==0&&value[newcoldrow*width+insertnocolumn].isback==false){
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].src=g1;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].dst=g2;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].index=k;
                        value[newcoldrow*width+insertnocolumn].gmatrix[cellpos].basketidx=bucketpos;
                        availbasket[newcoldrow*width+insertnocolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        value[bucketpos].next.push_back(newcoldrow*width+insertnocolumn);
                        return true;
                    }
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            insertnocolumn=(insertnocolumn+1)%width;
        }
    }
    int newbucketpos=findempty(bucketpos);
    if(newbucketpos==-1){
        cout<<" return bucket failed"<<endl;
        return false;
    }
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[newbucketpos].gmatrix[cellpos].src==0){
            value[newbucketpos].gmatrix[cellpos].src=g1;
            value[newbucketpos].gmatrix[cellpos].dst=g2;
            value[newbucketpos].gmatrix[cellpos].weight=weight;
            value[newbucketpos].gmatrix[cellpos].index=i;
            value[newbucketpos].gmatrix[cellpos].basketidx=bucketpos;
            availbasket[newbucketpos].globaluse++;
            rowsuse[newbucketpos/width]++;
            value[bucketpos].next.push_back(newbucketpos);
            return true;
        }
        cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
    }
    return false;
}




bool Coral::giveback(int pos,double &alltime){
    value[pos].isback=true;
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[pos].gmatrix[i].src==0) continue;
        int cellpos=(i-value[pos].gmatrix[i].index+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        freshedgestore(value[pos].gmatrix[i].basketidx,cellpos,value[pos].gmatrix[i].src,value[pos].gmatrix[i].dst,value[pos].gmatrix[i].weight,alltime);
        value[pos].gmatrix[i].basketidx=0;
        value[pos].gmatrix[i].dst=0;
        value[pos].gmatrix[i].index=0;
        value[pos].gmatrix[i].src=0;
        value[pos].gmatrix[i].weight=0;
    }
    value[pos].isback=true;
    rowsuse[pos/width]=rowsuse[pos/width]-availbasket[pos].globaluse+16;
    availbasket[pos].globaluse=0;
    return true;

}

//Debug version
bool Coral::givebackc(int pos,double &alltime){
    value[pos].isback=true;
    cout<<"clear pos:"<<pos<<endl;
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        cout<<"i:"<<i<<" coral.value[19956].next.size() :"<<value[19956].next.size()<<endl;;
        if(value[pos].gmatrix[i].src==0) continue;
        int cellpos=(i-value[pos].gmatrix[i].index+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        cout<<"give back()"<<cellpos<<endl;
        cout<<"start give back "<<value[pos].gmatrix[i].basketidx<<endl;
        freshedgestorec(value[pos].gmatrix[i].basketidx,cellpos,value[pos].gmatrix[i].src,value[pos].gmatrix[i].dst,value[pos].gmatrix[i].weight,alltime);
        cout<<"give back successful"<<endl;
        value[pos].gmatrix[i].basketidx=0;
        value[pos].gmatrix[i].dst=0;
        value[pos].gmatrix[i].index=0;
        value[pos].gmatrix[i].src=0;
        value[pos].gmatrix[i].weight=0;
    }
    value[pos].isback=true;
    rowsuse[pos/width]=rowsuse[pos/width]-availbasket[pos].globaluse+16;
    availbasket[pos].globaluse=0;
    return true;

}


void Coral:: printused(){
    for(int i=0;i<width*width;i++){
        for(int j=0;j<width;j++){
            cout<<availbasket[i*width+j].localuse<<" "<<availbasket[i*width+j].globaluse;
        }
        cout<<endl;
        
    }
}

void Coral::printoralused(string out_put_file_path){
    ofstream of(out_put_file_path);
    for(int i=0;i<width;i++){
        for(int j=0;j<width;j++){
            of<<oralused[i*width+j]<<" ";
        }
        of<<endl;
    }
    return;

}



void Coral::printavailable(string out_put_file_path){
    ofstream of(out_put_file_path);
    for(int i=0;i<width;i++){
        for(int j=0;j<width;j++){
            of<<availbasket[i*width+j].globaluse+availbasket[i*width+j].localuse<<" ";
        }
        of<<endl;
    }
    return;

}


void Coral::printlocaluse(string out_put_file_path){
    ofstream of(out_put_file_path);
    for(int i=0;i<width;i++){
        for(int j=0;j<width;j++){
            of<<availbasket[i*width+j].localuse<<" ";
        }
        of<<endl;
    }
    return;

}


void Coral::printglobaluse(string out_put_file_path){
    ofstream of(out_put_file_path);
    for(int i=0;i<width;i++){
        for(int j=0;j<width;j++){
            of<<availbasket[i*width+j].globaluse<<" ";
        }
        of<<endl;
    }
    return;

}


void Coral::printkinds(string path){
    ofstream of(path);
    for(int i=0;i<width;i++){
        for(int j=0;j<width;j++){
            of<<kinds[i*width+j]<<" ";
        }
        of<<endl;
    }
    return;

}


void Coral:: printpotentialhot(string path){
    ofstream of(path);
    for (auto it = hotrows.potentialhotrow.begin(); it != hotrows.potentialhotrow.end(); ++it) {
        of<<"row number: " << it->first<<" values: "<<it->second<<" ";
        of<<endl;
        
    }
    return ;

}

void Coral:: printmatch(string path){
    ofstream of(path);
    for (auto it = match.begin(); it != match.end(); ++it) {
        of<<"row number: " << it->first<<" match rows number: ";
        for(int num:it->second){
            of <<num<<" ";
        }
        of<<endl;
        
    }
    return ;

}

void Coral:: printbasicparameter(){
    cout<<"bucket matrix width :"<<width<<endl;
    cout<<"cell matrix width :"<<MatrixWidth<<endl;
    cout<<"fingerpoint stored :"<<fingerprint_length<<" bits, figerpoint to calculate: "<<fingerprint_length+Increasefingerpoint<<endl;
    cout<<"hot rows difined : 1.how many times we need ramdom occasion :"<<hotrows.CompareNumber<<endl;
    cout<<"                   2.how many occurrences of low - probability events :"<<hotrows.HotrowTimes<<endl;
}

bool Coral::insert(string s1, string s2, int weight)//insert funtion
{
    double alltime=0;

	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
	unsigned short g1 = hash1 & tmp;
	if(g1 == 0)  g1 += 1;
	unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
	unsigned short g2 = hash2 & tmp;
	if(g2==0)  g2 += 1;
	unsigned int h2 = (hash2 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c2=(hash2 >> fingerprint_length)%MatrixWidth;

	unsigned int k1 = (h1 << fingerprint_length) + g1;
	unsigned int k2 = (h2 << fingerprint_length) + g2;
    
    bool inserted=false;
    int pos=h1*width+h2;
    oralused[pos]++;
    int cellpos=c1*MatrixWidth+c2;
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[pos].lmatrix[cellpos].src==g1&&value[pos].lmatrix[cellpos].dst==g2&&value[pos].lmatrix[cellpos].index==i)
        {
            value[pos].lmatrix[cellpos].weight+=weight;
            inserted=true;
            return true;
        }
        if(value[pos].lmatrix[cellpos].src==0)
        {
            value[pos].lmatrix[cellpos].src=g1;
            value[pos].lmatrix[cellpos].dst=g2;
            value[pos].lmatrix[cellpos].weight=weight;
            hotrows.update(h1);
            kinds[pos]++;
            availbasket[pos].localuse++;
            rowsuse[h1]++;
            
            inserted=true;
            value[pos].lmatrix[cellpos].index=i;
            return true;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }
    
    //use own globall
    if(value[pos].isback==true||availbasket[pos].globaluse==0){
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            if(value[pos].gmatrix[cellpos].src==g1&&value[pos].gmatrix[cellpos].dst==g2&&value[pos].gmatrix[cellpos].index==i){
                value[pos].gmatrix[cellpos].weight+=weight;
                inserted=true;
                return true;
            }
            if(value[pos].gmatrix[cellpos].src==0){
                hotrows.update(h1);
                kinds[pos]++;
                availbasket[pos].globaluse++;
                rowsuse[h1]++;
                
                
                value[pos].gmatrix[cellpos].src=g1;
                value[pos].gmatrix[cellpos].dst=g2;
                value[pos].gmatrix[cellpos].weight=weight;
                value[pos].gmatrix[cellpos].basketidx=pos;
                
                inserted=true;
                value[pos].gmatrix[cellpos].index=i;
                value[pos].isback=true;
                return true;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }
    }

    else{
        giveback(pos,alltime);
        value[pos].isback=true;
        value[pos].gmatrix[cellpos].src=g1;
        value[pos].gmatrix[cellpos].dst=g2;
        value[pos].gmatrix[cellpos].weight=weight;
        
        hotrows.update(h1);
        kinds[pos]++;
        availbasket[pos].globaluse++;
        
        rowsuse[h1]++;
        
        
        
        value[pos].gmatrix[cellpos].index=0;
        inserted=true;
        return true;

    }

    //find in used bucket
    for(int nextpos:value[pos].next){
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            if(value[nextpos].gmatrix[cellpos].src==g1&&value[nextpos].gmatrix[cellpos].dst==g2&&value[nextpos].gmatrix[cellpos].index==i){
                value[nextpos].gmatrix[cellpos].weight+=weight;
                inserted=true;
                return true;
            }
            if(value[nextpos].gmatrix[cellpos].src==0){
                value[nextpos].gmatrix[cellpos].src=g1;
                value[nextpos].gmatrix[cellpos].dst=g2;
                value[nextpos].gmatrix[cellpos].weight=weight;
                value[nextpos].gmatrix[cellpos].basketidx=pos;

                
                kinds[pos]++;
                availbasket[nextpos].globaluse++;
                rowsuse[nextpos/width]++;
                hotrows.update(nextpos/width);
                
                value[nextpos].gmatrix[cellpos].index=i;
                inserted=true;
                return true;


            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }

    }

    int nextcolumn=h2;
    //need a new bucket to insert

    if(match[h1].size()!=0){
       int lastcoldrow=match[h1][match[h1].size()-1]; 
       for(int i=0;i<width;i++){
            if(value[lastcoldrow*width+nextcolumn].isback==true) {
                continue;
            }
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                if(value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].src==0){
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;
                
                    availbasket[lastcoldrow*width+nextcolumn].globaluse++;
                    rowsuse[lastcoldrow]++;
                    hotrows.update(lastcoldrow);
                    
                    value[pos].next.push_back(lastcoldrow*width+nextcolumn);
                    return true;
                
                }
                cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
            }
            nextcolumn=(nextcolumn+1)%width;
        }
       
    }
    


    //need a new cold row
    int newcoldrow;

    //存在严格筛选的冷行
    if(getcoldrowstrict(h1,h2,newcoldrow,alltime)){
        for(int i=0;i<width;i++){
            if(value[newcoldrow*width+nextcolumn].isback==true) continue;
            if(availbasket[newcoldrow*width+nextcolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                    if(value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==0&&value[newcoldrow*width+nextcolumn].isback==false){
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;

                    
                        availbasket[newcoldrow*width+nextcolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        hotrows.update(newcoldrow);
                    
                        
                        value[pos].next.push_back(newcoldrow*width+nextcolumn);
                        return true;
                        
                    }
    
    
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            nextcolumn=(nextcolumn+1)%width;
        }

    }

    //have to use a potential hot row
    if(getcoldrow(h1,h2,newcoldrow,alltime)){
        for(int i=0;i<width;i++){
            if(value[newcoldrow*width+nextcolumn].isback==true) continue;
            if(availbasket[newcoldrow*width+nextcolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                    if(value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==0){
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;
                        

                        rowsuse[newcoldrow]++;
                        hotrows.update(newcoldrow);
                        availbasket[newcoldrow*width+nextcolumn].globaluse++;
                        

                        value[pos].next.push_back(newcoldrow*width+nextcolumn);
                        return true;
                        
                    }
    
    
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            nextcolumn=(nextcolumn+1)%width;
        }

    }

    //all rows is hot ,find a place to store
    int newbucketpos=findempty();
    
    if(newbucketpos==-1){
        cout<<" insert failed"<<endl;
        return true;
    }
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[newbucketpos].gmatrix[cellpos].src==0){
            value[newbucketpos].gmatrix[cellpos].src==g1;
            value[newbucketpos].gmatrix[cellpos].dst==g2;
            value[newbucketpos].gmatrix[cellpos].weight=weight;
            value[newbucketpos].gmatrix[cellpos].index==i;
            availbasket[newbucketpos].globaluse++;
            rowsuse[newbucketpos/width]++;
            hotrows.update(newbucketpos/width);
            value[pos].next.push_back(newbucketpos);
            return true;
        }
        cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
    }


    cout<<"insert failed"<<endl;
    return true;
}




double Coral::insertTime(string s1, string s2, int weight)//with  parallel program 
{
    timeval ts,te;
    double alltime=0;
    
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
	unsigned short g1 = hash1 & tmp;
	if(g1 == 0)  g1 += 1;
	unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
	unsigned short g2 = hash2 & tmp;
	if(g2==0)  g2 += 1;
	unsigned int h2 = (hash2 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c2=(hash2 >> fingerprint_length)%MatrixWidth;

	unsigned int k1 = (h1 << fingerprint_length) + g1;
	unsigned int k2 = (h2 << fingerprint_length) + g2;
    
    bool inserted=false;
    int pos=h1*width+h2;
    oralused[pos]++;
    //cout<<"pos:"<<pos<<endl;
    int cellpos=c1*MatrixWidth+c2;
    //cout<<"cellpos:"<<cellpos<<endl;
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        //cout<<"i: "<<i<<endl;
        //cout<<g1<<endl;
        //cout<<value[pos].matrix[cellpos].src<<endl;
        if(value[pos].lmatrix[cellpos].src==g1&&value[pos].lmatrix[cellpos].dst==g2&&value[pos].lmatrix[cellpos].index==i)
        {
            //cout<<"已经有，存到这里"<<endl;
            value[pos].lmatrix[cellpos].weight+=weight;
            inserted=true;
            return alltime;
        }
        if(value[pos].lmatrix[cellpos].src==0)
        {
            value[pos].lmatrix[cellpos].src=g1;
            value[pos].lmatrix[cellpos].dst=g2;
            value[pos].lmatrix[cellpos].weight=weight;
            gettimeofday(&ts,NULL);
            hotrows.update(h1);
            kinds[pos]++;
            availbasket[pos].localuse++;
            rowsuse[h1]++;
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            inserted=true;
            value[pos].lmatrix[cellpos].index=i;
            return alltime;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }
    //
    //自己basket的local部分用完了 开始用自己的global部分
    if(value[pos].isback==true||availbasket[pos].globaluse==0){//自己的global区域已经收回或者是自己的global还没有被任何使用
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            if(value[pos].gmatrix[cellpos].src==g1&&value[pos].gmatrix[cellpos].dst==g2&&value[pos].gmatrix[cellpos].index==i){
                value[pos].gmatrix[cellpos].weight+=weight;
                inserted=true;
                return alltime;
            }
            if(value[pos].gmatrix[cellpos].src==0){
                gettimeofday(&ts,NULL);
                hotrows.update(h1);
                kinds[pos]++;
                availbasket[pos].globaluse++;
                gettimeofday(&te,NULL);
                rowsuse[h1]++;
                alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
                
                
                value[pos].gmatrix[cellpos].src=g1;
                value[pos].gmatrix[cellpos].dst=g2;
                value[pos].gmatrix[cellpos].weight=weight;
                value[pos].gmatrix[cellpos].basketidx=pos;
                
                inserted=true;
                value[pos].gmatrix[cellpos].index=i;
                value[pos].isback=true;
                return alltime;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }
    }

    else{
        giveback(pos,alltime);//将属于自己的global清空
        value[pos].isback=true;
        value[pos].gmatrix[cellpos].src=g1;
        value[pos].gmatrix[cellpos].dst=g2;
        value[pos].gmatrix[cellpos].weight=weight;
        gettimeofday(&ts,NULL);
        hotrows.update(h1);
        kinds[pos]++;
        availbasket[pos].globaluse++;
        gettimeofday(&te,NULL);
        rowsuse[h1]++;
        alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        
        
        value[pos].gmatrix[cellpos].index=0;
        inserted=true;
        return alltime;

    }

    //在自己已经选用的bucket里面找找
    for(int nextpos:value[pos].next){
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            if(value[nextpos].gmatrix[cellpos].src==g1&&value[nextpos].gmatrix[cellpos].dst==g2&&value[nextpos].gmatrix[cellpos].index==i){
                value[nextpos].gmatrix[cellpos].weight+=weight;
                inserted=true;
                return alltime;
            }
            if(value[nextpos].gmatrix[cellpos].src==0){
                value[nextpos].gmatrix[cellpos].src=g1;
                value[nextpos].gmatrix[cellpos].dst=g2;
                value[nextpos].gmatrix[cellpos].weight=weight;
                value[nextpos].gmatrix[cellpos].basketidx=pos;

                gettimeofday(&ts,NULL);
                kinds[pos]++;
                availbasket[nextpos].globaluse++;
                rowsuse[nextpos/width]++;
                hotrows.update(nextpos/width);
                gettimeofday(&te,NULL);
                alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
                
                value[nextpos].gmatrix[cellpos].index=i;
                inserted=true;
                return alltime;


            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }

    }

    int nextcolumn=h2;
    //如果到了这一步 说明自己basket的local和global区域都没有位置插入 已经选用的bucket里面也没有这样的边

    if(match[h1].size()!=0){
       int lastcoldrow=match[h1][match[h1].size()-1]; 
       for(int i=0;i<width;i++){
            if(value[lastcoldrow*width+nextcolumn].isback==true) {
                continue;//如果识别到这个bucket已经被收回就不能放在这个bucket里面了
            }
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                if(value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].src==0){
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                    value[lastcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;
                
                    gettimeofday(&ts,NULL);
                    availbasket[lastcoldrow*width+nextcolumn].globaluse++;
                    rowsuse[lastcoldrow]++;
                    hotrows.update(lastcoldrow);
                    gettimeofday(&te,NULL);
                    alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
                    value[pos].next.push_back(lastcoldrow*width+nextcolumn);
                    return alltime;
                
                }
                cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
            }
            nextcolumn=(nextcolumn+1)%width;
        }
       
    }
    


    //如果到了这里说明已经为该热行匹配的冷行也没有位置存储 需要再给他匹配一个冷行
    int newcoldrow;

    //存在严格筛选的冷行
    if(getcoldrowstrict(h1,h2,newcoldrow,alltime)){
        for(int i=0;i<width;i++){
            if(value[newcoldrow*width+nextcolumn].isback==true) continue;//如果识别到这个bucket已经被收回就不能放在这个bucket里面了
            if(availbasket[newcoldrow*width+nextcolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                    if(value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==0&&value[newcoldrow*width+nextcolumn].isback==false){
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;

                        gettimeofday(&ts,NULL);
                        availbasket[newcoldrow*width+nextcolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        hotrows.update(newcoldrow);
                        gettimeofday(&te,NULL);
                        alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
                        
                        value[pos].next.push_back(newcoldrow*width+nextcolumn);
                        return alltime;
                        
                    }
    
    
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            nextcolumn=(nextcolumn+1)%width;
        }

    }

    //要用潜在热行存了
    if(getcoldrow(h1,h2,newcoldrow,alltime)){
        for(int i=0;i<width;i++){
            if(value[newcoldrow*width+nextcolumn].isback==true) continue;//如果识别到这个bucket已经被收回就不能放在这个bucket里面了
            if(availbasket[newcoldrow*width+nextcolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                    if(value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==0){
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;
                        


                        gettimeofday(&ts,NULL);
                        rowsuse[newcoldrow]++;
                        hotrows.update(newcoldrow);
                        availbasket[newcoldrow*width+nextcolumn].globaluse++;
                        gettimeofday(&te,NULL);
                        alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
                        

                        value[pos].next.push_back(newcoldrow*width+nextcolumn);
                        return alltime;
                        
                    }
    
    
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            nextcolumn=(nextcolumn+1)%width;
        }

    }

    //现在所有行都比较热门了 要用findpos（）去找一个空的cell 存了就行
    //int newbucketpos=findempty();
    int newbucketpos=findempty(pos);//调试用
    if(newbucketpos==-1){
        cout<<" insert failed"<<endl;
        return alltime;
    }
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[newbucketpos].gmatrix[cellpos].src==0){
            value[newbucketpos].gmatrix[cellpos].src==g1;
            value[newbucketpos].gmatrix[cellpos].dst==g2;
            value[newbucketpos].gmatrix[cellpos].weight=weight;
            value[newbucketpos].gmatrix[cellpos].index==i;
            availbasket[newbucketpos].globaluse++;
            rowsuse[newbucketpos/width]++;
            hotrows.update(newbucketpos/width);
            value[pos].next.push_back(newbucketpos);
            return alltime;
        }
        cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
    }


    cout<<"insert failed"<<endl;
    return alltime;
}


/*  insert debug fuction */
bool Coral::insertc(string s1, string s2, int weight)
{

    double alltime=0;
    cout<<"insertc() "<<endl;
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	//float a=2;
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
	unsigned short g1 = hash1 & tmp;
	if(g1 == 0)  g1 += 1;
	unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
	unsigned short g2 = hash2 & tmp;
	if(g2==0)  g2 += 1;
	unsigned int h2 = (hash2 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c2=(hash2 >> fingerprint_length)%MatrixWidth;

	unsigned int k1 = (h1 << fingerprint_length) + g1;
	unsigned int k2 = (h2 << fingerprint_length) + g2;
    
    bool inserted=false;
    int pos=h1*width+h2;
    oralused[pos]++;
    cout<<"pos:"<<pos<<endl;
    int cellpos=c1*MatrixWidth+c2;
    cout<<"cellpos:"<<cellpos<<endl;
    cout<<"search in local"<<endl;
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[pos].lmatrix[cellpos].src==g1&&value[pos].lmatrix[cellpos].dst==g2&&value[pos].lmatrix[cellpos].index==i)
        {
            cout<<"already have "<<endl;
            value[pos].lmatrix[cellpos].weight+=weight;
            inserted=true;
            return true;
        }
        if(value[pos].lmatrix[cellpos].src==0)
        {
            cout<<"find a new space to store"<<endl;
            kinds[pos]++;
            value[pos].lmatrix[cellpos].src=g1;
            value[pos].lmatrix[cellpos].dst=g2;
            value[pos].lmatrix[cellpos].weight=weight;
            availbasket[pos].localuse++;
            rowsuse[h1]++;
            inserted=true;
            value[pos].lmatrix[cellpos].index=i;
            return true;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }

    cout<<"search in own global"<<endl;
    cout<<"value[pos].isback : "<<value[pos].isback<<endl;
    cout<<"availbasket[pos].globaluse: "<<availbasket[pos].globaluse<<endl;
    if(value[pos].isback==true||availbasket[pos].globaluse==0){
        
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            
            if(value[pos].gmatrix[cellpos].src==g1&&value[pos].gmatrix[cellpos].dst==g2&&value[pos].gmatrix[cellpos].index==i){
                cout<<"already have in own global"<<endl;
                value[pos].gmatrix[cellpos].weight+=weight;
                inserted=true;
                return true;
            }
            if(value[pos].gmatrix[cellpos].src==0){
                cout<<"find a new space in own global"<<endl;
                kinds[pos]++;
                value[pos].gmatrix[cellpos].src=g1;
                value[pos].gmatrix[cellpos].dst=g2;
                value[pos].gmatrix[cellpos].weight=weight;
                value[pos].gmatrix[cellpos].basketidx=pos;
                availbasket[pos].globaluse++;
                inserted=true;
                value[pos].gmatrix[cellpos].index=i;
                value[pos].isback=true;
                return true;
            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }
    }

    else{
        cout<<"other bucket has used this scope and we should give it back"<<endl;
        givebackc(pos,alltime);
        cout<<"clear own global and store"<<endl;
        value[pos].isback=true;
        value[pos].gmatrix[cellpos].src=g1;
        value[pos].gmatrix[cellpos].dst=g2;
        value[pos].gmatrix[cellpos].weight=weight;
        kinds[pos]++;
        availbasket[pos].globaluse++;
        value[pos].gmatrix[cellpos].index=0;
        inserted=true;
        return true;

    }

    cout<<"search in others' bucket"<<endl;
    cout<<"other bucket number:"<<value[pos].next.size();
    for(int a:value[pos].next){
        cout<<a<<" ";
    }
    for(int nextpos:value[pos].next){
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            if(value[nextpos].gmatrix[cellpos].src==g1&&value[nextpos].gmatrix[cellpos].dst==g2&&value[nextpos].gmatrix[cellpos].index==i){
                cout<<"already have in other"<<endl;
                value[nextpos].gmatrix[cellpos].weight+=weight;
                inserted=true;
                return true;

            }
            if(value[nextpos].gmatrix[cellpos].src==0){
                cout<<"get a new space in others"<<endl;
                kinds[pos]++;
                value[nextpos].gmatrix[cellpos].src=g1;
                value[nextpos].gmatrix[cellpos].dst=g2;
                value[nextpos].gmatrix[cellpos].weight=weight;
                value[nextpos].gmatrix[cellpos].basketidx=pos;
                availbasket[nextpos].globaluse++;
                rowsuse[nextpos/width]++;
                value[nextpos].gmatrix[cellpos].index=i;
                inserted=true;
                return true;


            }
            cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
        }

    }

    int nextcolumn=h2;
    cout<<"need find a new bucket and should search in one row first"<<endl;

    if(!match[h1].empty()){
        int nextrow=match[h1][match[h1].size()-1];
        for(int i=0;i<width;i++){
            cout<<"search in rows' other bucket"<<endl;
            if(value[nextrow*width+nextcolumn].isback==true) continue;
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                if(value[nextrow*width+nextcolumn].gmatrix[cellpos].src==g1&&value[nextrow*width+nextcolumn].gmatrix[cellpos].dst==g2&&value[nextrow*width+nextcolumn].gmatrix[cellpos].index==j){
                    value[nextrow*width+nextcolumn].gmatrix[cellpos].weight+=weight;
                    return true;
                }
                if(value[nextrow*width+nextcolumn].gmatrix[cellpos].src==0){
                    value[nextrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                    value[nextrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                    value[nextrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                    value[nextrow*width+nextcolumn].gmatrix[cellpos].index==j;
                    availbasket[nextrow*width+nextcolumn].globaluse++;
                    rowsuse[nextrow]++;
                    value[pos].next.push_back(nextrow*width+nextcolumn);
                    return true;
                    
                }


                cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
            }
            nextcolumn=(nextcolumn+1)%width;

        }

    }
    

    int newcoldrow;

    cout<<"we need search a new cold row"<<endl;
    if(getcoldrowstrict_fixed(h1,h2,newcoldrow,alltime)){
        for(int i=0;i<width;i++){
            if(value[newcoldrow*width+nextcolumn].isback==true) continue;
            if(availbasket[newcoldrow*width+nextcolumn].globaluse<(MatrixWidth*MatrixWidth)){
                for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                    if(value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==0&&value[newcoldrow*width+nextcolumn].isback==false){
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;
                        availbasket[newcoldrow*width+nextcolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        value[pos].next.push_back(newcoldrow*width+nextcolumn);
                        return true;
                        
                    }
    
    
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            nextcolumn=(nextcolumn+1)%width;
        }

    }
    if(getcoldrow_fixed(h1,h2,newcoldrow,alltime)){
        cout<<"get a new cold row "<<newcoldrow<<endl;
        for(int i=0;i<width;i++){
            if(value[newcoldrow*width+nextcolumn].isback==true) continue;
            if(availbasket[newcoldrow*width+nextcolumn].globaluse<(MatrixWidth*MatrixWidth)){
                cout<<"this bucket have more space to store"<<endl;
                for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                    if(value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==0){
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].src==g1;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].dst==g2;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].weight=weight;
                        value[newcoldrow*width+nextcolumn].gmatrix[cellpos].index==j;
                        availbasket[newcoldrow*width+nextcolumn].globaluse++;
                        rowsuse[newcoldrow]++;
                        value[pos].next.push_back(newcoldrow*width+nextcolumn);
                        return true;
                        
                    }
    
    
                    cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
                }
            }
            nextcolumn=(nextcolumn+1)%width;
        }

    }
    cout<<"just find a space without any conditions"<<endl;
    int newbucketpos=findempty();
    if(newbucketpos==-1){
        cout<<" insert failed"<<endl;
        return false;
    }
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[newbucketpos].gmatrix[cellpos].src==0){
            value[newbucketpos].gmatrix[cellpos].src==g1;
            value[newbucketpos].gmatrix[cellpos].dst==g2;
            value[newbucketpos].gmatrix[cellpos].weight=weight;
            value[newbucketpos].gmatrix[cellpos].index==i;
            availbasket[newbucketpos].globaluse++;
            rowsuse[newbucketpos/width]++;
            value[pos].next.push_back(newbucketpos);
            return true;
        }
        cellpos=(cellpos+1+MatrixWidth*MatrixWidth)%(MatrixWidth*MatrixWidth);
    }

    

    cout<<"insert failed"<<endl;
    return false;
}









int Coral::edgeWeightQuery(string s1, string s2){

    
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	//float a=2;
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
	unsigned short g1 = hash1 & tmp;
	if(g1 == 0)  g1 += 1;
	unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
	uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
    unsigned short g2 = hash2 & tmp;
	if(g2==0)  g2 += 1;
	unsigned int h2 = (hash2 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c2=(hash2 >> fingerprint_length)%MatrixWidth;

	unsigned int k1 = (h1 << fingerprint_length) + g1;
	unsigned int k2 = (h2 << fingerprint_length) + g2;

    int pos=h1*width+h2;
    int cellpos=c1*MatrixWidth+c2;
    

    //query in local
    
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[pos].lmatrix[cellpos].src==g1&&value[pos].lmatrix[cellpos].dst==g2&&value[pos].lmatrix[cellpos].index==i)return value[pos].lmatrix[cellpos].weight;
        if(value[pos].lmatrix[cellpos].src==0)return 0;
        cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
    }

    if(value[pos].isback==false) return 0;

    //query in global
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[pos].gmatrix[cellpos].src==g1&&value[pos].gmatrix[cellpos].dst==g2&&value[pos].gmatrix[cellpos].index==i)return value[pos].gmatrix[cellpos].weight;
        if(value[pos].lmatrix[cellpos].src==0)return 0;
        cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);

    }

    //query in next basket's global
    for(int nextpos:value[pos].next){
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            if(value[nextpos].gmatrix[cellpos].src==g1&&value[nextpos].gmatrix[cellpos].dst==g2&&value[nextpos].gmatrix[cellpos].index==i)return value[nextpos].gmatrix[cellpos].weight;
            cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
        }
    }

    return 0;
}



int Coral::nodeWeightQuery(string s1,int type){
    
    unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
    unsigned short g1 = hash1 & tmp;
    if(g1 == 0)  g1 += 1;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
    unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    int weight=0;
    
    //out-flow
    int pos1=h1*width;
    int pos;
    int cellpos=c1*MatrixWidth;
    

    if(type==0){
        for(int i=0;i<width;i++){
            pos=pos1+i;
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){  
                          
                if(value[pos].lmatrix[cellpos].src==g1){
                    weight+=value[pos].lmatrix[cellpos].weight;
                }
                //if(value[pos].lmatrix[cellpos].src==0)break;
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].isback==false) continue;
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                if(value[pos].gmatrix[cellpos].src==g1){
                    weight+=value[pos].gmatrix[cellpos].weight;
                }
                //if(value[pos].gmatrix[cellpos].src==0)break;
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].next.empty())continue;
            for(int nextpos:value[pos].next){
                for(int m=0;m<MatrixWidth*MatrixWidth;m++){
                    if(value[nextpos].gmatrix[cellpos].src==g1){
                        weight+=value[nextpos].gmatrix[cellpos].weight;
                    }
                    //if(value[nextpos].gmatrix[cellpos].src==0)break;
                    cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
                }
            }
           
        }
        return weight;
        
    }

    pos1=h1;
    cellpos=c1;
    
    if(type==1){
        for(int i=0;i<width;i++){
            
            pos=i*width+pos1;
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                if(value[pos].lmatrix[j].dst==g1){
                    
                    weight+=value[pos].lmatrix[j].weight;
                }  
            }   
  
            if(value[pos].isback==false) continue;
            
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                if(value[pos].gmatrix[k].dst==g1){
                    weight+=value[pos].gmatrix[k].weight;
                }   
                
            }   
  
            if(value[pos].next.empty())continue;
            
            for(int nextpos:value[pos].next){
                for(int m=0;m<MatrixWidth*MatrixWidth;m++){
                    if(value[nextpos].gmatrix[m].dst==g1){
                        weight+=value[nextpos].gmatrix[m].weight;
                    }   
                    
                }   
            }
            
        }   
        return weight;

    }



    return 0;
}




int Coral::nodeDegreeQuery(string s1,int type){
    
    unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
    unsigned short g1 = hash1 & tmp;
    if(g1 == 0)  g1 += 1;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
    unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    int weight=0;
    
    //out-flow
    int pos1=h1*width;
    int pos;
    int cellpos=c1*MatrixWidth;
    

    if(type==0){
        for(int i=0;i<width;i++){
            pos=pos1+i;
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){  
                          
                if(value[pos].lmatrix[cellpos].src==g1){
                    weight++;
                }
                
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].isback==false) continue;
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                if(value[pos].gmatrix[cellpos].src==g1){
                    weight++;
                }
                
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].next.empty())continue;
            for(int nextpos:value[pos].next){
                for(int m;m<MatrixWidth*MatrixWidth;m++){
                    if(value[nextpos].gmatrix[cellpos].src==g1){
                        weight++;
                    }
                    
                    cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
                }
            }
           
        }
        return weight;
        
    }

    pos1=h1;
    cellpos=c1;
    //in-flow
    
    if(type==1){
        for(int i=0;i<width;i++){
            
            pos=i*width+pos1;
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                
                if(value[pos].lmatrix[j].dst==g1){
                    weight++;
                }  
            }   
  
            if(value[pos].isback==false) continue;
            
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                if(value[pos].gmatrix[k].dst==g1){
                    weight++;
                }   
            }   
  
            if(value[pos].next.empty())continue;            
            for(int nextpos:value[pos].next){
                for(int m=0;m<MatrixWidth*MatrixWidth;m++){
                    if(value[nextpos].gmatrix[m].dst==g1){
                        weight++;
                    }                     
                }   
            }           
        }   
        return weight;
    }



    return 0;
}



int Coral::nodeDegreeQuery(infoincell s1,int type){//if query node had in coral,s1 is the information we known
    unsigned short g1 = s1.fingerpoint;
    if(g1 == 0)  g1 += 1;
    uint8_t c1=s1.cellindex;
    unsigned int h1 = s1.index;
    int degree=0;
    int pos=h1*width;
    int cellpos=c1*MatrixWidth;
    if(type==0){
        for(int i=0;i<width;i++){
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                if(value[pos].lmatrix[cellpos].src==g1){
                degree++;
                }
                if(value[pos].lmatrix[cellpos+j].src==0)break;
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].isback==false) break;
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                if(value[pos].gmatrix[cellpos].src==g1){
                degree++;
                }
                if(value[pos].gmatrix[cellpos].src==0)break;
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].next.empty())break;
            for(int nextpos:value[pos].next){
                for(int m;m<MatrixWidth*MatrixWidth;m++){
                    if(value[nextpos].gmatrix[cellpos].src==g1){
                    degree++;
                    }
                    if(value[nextpos].gmatrix[cellpos].src==0)break;
                    cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
                }
            }
            pos=pos+1;
        }
        return degree;
        
    }

    pos=h1;
    cellpos=c1;
    if(type==1){
        for(int i=0;i<width;i++){
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                if(value[pos].lmatrix[cellpos].dst==g1){
                degree++;
                }   
                if(value[pos].lmatrix[cellpos+j].src==0)break;
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }   
  
            if(value[pos].isback==false) break;
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                if(value[pos].gmatrix[cellpos].dst==g1){
                degree++;
                }   
                if(value[pos].gmatrix[cellpos].src==0)break;
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }   
  
            if(value[pos+i].next.empty())break;
            for(int nextpos:value[pos].next){
                for(int m;m<MatrixWidth*MatrixWidth;m++){
                    if(value[nextpos].gmatrix[cellpos].dst==g1){
                    degree++;
                    }   
                    if(value[nextpos].gmatrix[cellpos].src==0)break;
                    cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
                }   
            }
            pos=width+pos;
        }   
          return degree;

    }


    cout<<"input type error"<<endl;
    return 0;    
}



bool Coral::reachabilityQuery(string s1,string s2){
    unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	int tmp = pow(2.0, fingerprint_length) - 1;
	unsigned short g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
	unsigned short g2 = hash2 & tmp;
	if (g2 == 0) g2 += 1;
    unsigned int h2 = (hash2 >> (fingerprint_length+Increasefingerpoint)) % width;
    uint8_t c2=(hash2 >>fingerprint_length)%MatrixWidth;



    int pos=h1*width+h2;
    int cellpos=c1*MatrixWidth+c2;
    
    //query in local
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[pos].lmatrix[cellpos].src==g1&&value[pos].lmatrix[cellpos].dst==g2&&value[pos].lmatrix[cellpos].index==i)return true;
        if(value[pos].lmatrix[cellpos].src==0)return false;
        cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
    }

    if(value[pos].isback==false) return false;

    //query in global
    for(int i=0;i<MatrixWidth*MatrixWidth;i++){
        if(value[pos].gmatrix[cellpos].src==g1&&value[pos].gmatrix[cellpos].dst==g2&&value[pos].gmatrix[cellpos].index==i)return true;
        if(value[pos].lmatrix[cellpos].src==0)return false;
        cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);

    }

    //query in next bucket global
    for(int nextpos:value[pos].next){
        for(int i=0;i<MatrixWidth*MatrixWidth;i++){
            if(value[nextpos].gmatrix[cellpos].src==g1&&value[nextpos].gmatrix[cellpos].dst==g2&&value[nextpos].gmatrix[cellpos].index==i)return true;
            cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
        }
    }

    return false;




}

void Coral::findnextnode(string s1,vector<infoincell> &nodes){//find nearby nodes,record neighbors' information
    unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
    unsigned short g1 = hash1 & tmp;
    if(g1 == 0)  g1 += 1;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
    unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    int weight=0;
    
    
    int pos1=h1*width;
    int cellpos=c1*MatrixWidth;
    int pos;
   
    
        
        for(int i=0;i<width;i++){
            
            pos=pos1+i;
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){
                          
                if(value[pos].lmatrix[cellpos].src==g1){
                    infoincell a;
                    a.cellindex=(cellpos-value[pos].lmatrix[cellpos].index+MatrixWidth*MatrixWidth)%MatrixWidth;
                    a.index=i;
                    a.fingerpoint=value[pos].lmatrix[cellpos].dst;
                    nodes.push_back(a);
                    weight++;
                }
                
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }
            
            if(value[pos].isback==false) continue;
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                
                if(value[pos].gmatrix[cellpos].src==g1){
                    infoincell b;
                    b.cellindex=(cellpos-value[pos].gmatrix[cellpos].index+MatrixWidth*MatrixWidth)%MatrixWidth;
                    b.index=i;
                    b.fingerpoint=value[pos].gmatrix[cellpos].dst;
                    nodes.push_back(b);
                    weight++;
                }
                
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].next.empty())continue;
            for(int nextpos:value[pos].next){
                
                for(int m=0;m<MatrixWidth*MatrixWidth;m++){

                    if(value[nextpos].gmatrix[cellpos].src==g1){
                        infoincell c;
                        c.cellindex=(cellpos-value[pos].gmatrix[cellpos].index+MatrixWidth*MatrixWidth)%MatrixWidth;
                        c.index=value[pos].gmatrix[cellpos].basketidx%width;
                        c.fingerpoint=value[pos].gmatrix[cellpos].dst;
                        nodes.push_back(c);
                        weight++;
                    }
                    
                    cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
                }
            }
           
        }
        
        return ;
        
    
}


void Coral::findnextnode(infoincell s,vector<infoincell> &nodes){
    int pos1=s.index*width;
    int cellpos=s.cellindex*MatrixWidth;
    uint16_t g1=s.fingerpoint;
    int weight=0;
    int pos;
   
    
    
        for(int i=0;i<width;i++){
            pos=pos1+i;
            for(int j=0;j<MatrixWidth*MatrixWidth;j++){  
                          
                if(value[pos].lmatrix[cellpos].src==g1){
                    infoincell a;
                    a.cellindex=(cellpos-value[pos].lmatrix[cellpos].index+MatrixWidth)%MatrixWidth;
                    a.index=i;
                    a.fingerpoint=value[pos].lmatrix[cellpos].dst;
                    nodes.push_back(a);
                    weight++;
                }
                
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].isback==false) continue;
            for(int k=0;k<MatrixWidth*MatrixWidth;k++){
                if(value[pos].gmatrix[cellpos].src==g1){
                    infoincell b;
                    b.cellindex=(cellpos-value[pos].gmatrix[cellpos].index+MatrixWidth)%MatrixWidth;
                    b.index=i;
                    b.fingerpoint=value[pos].gmatrix[cellpos].dst;
                    nodes.push_back(b);
                    weight++;
                }
                
                cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
            }

            if(value[pos].next.empty())continue;
            for(int nextpos:value[pos].next){
                for(int m;m<MatrixWidth*MatrixWidth;m++){
                    if(value[nextpos].gmatrix[cellpos].src==g1){
                        infoincell c;
                        c.cellindex=(cellpos-value[pos].gmatrix[cellpos].index+MatrixWidth)%MatrixWidth;
                        c.index=value[pos].gmatrix[cellpos].basketidx%width;
                        c.fingerpoint=value[pos].gmatrix[cellpos].dst;
                        nodes.push_back(c);
                        weight++;
                    }
                    
                    cellpos=(cellpos+1)%(MatrixWidth*MatrixWidth);
                }
            }
           
        }
        return ;
        
    
}

int Coral::findnearbyvertex(string s1,int k){
    unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
    unsigned int tmp = pow(2.0, fingerprint_length) - 1;
    unsigned short g1 = hash1 & tmp;
    if(g1 == 0)  g1 += 1;
    uint8_t c1=(hash1 >> fingerprint_length)%MatrixWidth;
    unsigned int h1 = (hash1 >> (fingerprint_length+Increasefingerpoint)) % width;
    
    infoincell query;
    query.index=h1;
    query.cellindex=c1;
    query.fingerpoint=g1;
    int degree=0;
    if(k<1){
        cout<<"this query is illegal"<<endl;
        return 0;
    }
   
    vector<infoincell> resultnode,increasenode,tem,temall;
    findnextnode(s1,resultnode);
    increasenode=resultnode;
    //cout<<"increase num:"<<increasenode.size();
    if(k==1) return resultnode.size();
    int resnum=0;
    resnum+=resultnode.size();
    for(int i=0;i<k-2;i++){
        //cout<<" i:"<<i;
       
        int increasenum=0;
        for(infoincell first:increasenode){
            increasenum++;
            //cout<<" id :"<<increasenum;
            tem.clear();
            findnextnode(first,tem);
            //cout<<"this time increase num"<<tem.size();
            temall.insert(temall.end(),tem.begin(),tem.end());
            resnum+=tem.size();
            
        }
        increasenode.clear();
        increasenode.insert(increasenode.end(), temall.begin(), temall.end());
        temall.clear();
        
    }
    
    return resnum;
}

void Coral::TopKheavyhitter(vector<string> &lotsnode){
    int maxnum=0;
    string maxnode;
    for(string s:lotsnode){
        if(this->nodeWeightQuery(s,1)>maxnum){
            maxnum=this->nodeWeightQuery(s,1);
            maxnode=s;
        }
        
    }
}

#endif // CORAL_H
