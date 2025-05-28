#ifndef QUERYFUNCTIONS__H
#define QUERYFUNCTIONS__H



#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <thread>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "Coral.h"

using namespace std;

struct querydata{
    unsigned int node1;
    unsigned int node2;
};

// Calculate the time difference between two `timespec` structures in nanoseconds.
long long timespec_diff(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000000000LL + (end->tv_nsec - start->tv_nsec);
}



/***************** function declaration ***********************/
// tool functions
//int isFolderExist(char* folder);
int createDirectory(char* sPathName);//Determine whether the folder can be retrieved normally.
uint64_t count_lines(string file);//file length
void progress_bar(int n);//Progress bar display
int readRandomFileToDataArray(const string& file, querydata dataArray[], int arraySize);
// insert functions
int insert(Coral& coral, const string &filename, int type);
int insertinfo(Coral& coral, const string &filename, int type,string datainfopath);
void count_num(uint64_t* datanum, uint64_t* end_num);
// query functions

unsigned int EdgeWeightQuery(Coral &coral,querydata dataArray[],int querysum);
unsigned int EdgeWeightQuery(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath);
//unsigned int EdgeWeightQueryCDF(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath,int times);
unsigned int NodeQuery(Coral &coral,querydata dataArray[],int querysum,int type);
unsigned int NodeQuery(Coral &coral,querydata dataArray[],int querysum,int type,string ResultFilePath);
unsigned int NodeDegreeQuery(Coral &coral,querydata dataArray[],int querysum,int type);
unsigned int NodeDegreeQuery(Coral &coral,querydata dataArray[],int querysum,int type,string ResultFilePath);
unsigned int ReachabilityQuery(Coral &coral,querydata dataArray[],int querysum);
unsigned int ReachabilityQuery(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath);

/***************** function declaration ***********************/


uint64_t count_lines(string file) {
    ifstream readFile;
    uint64_t n = 0;
    char line[512];
    string temp;
    readFile.open(file, ios::in);
    if(readFile.fail()) {
        cout << "error in opening file" << endl;
        return 0;
    }
    else {
        while(getline(readFile,temp))
            n++;
    }
    readFile.close();
    return n;
}


int createDirectory(char* sPathName) {
    char DirName[256];
    strcpy(DirName, sPathName);
    int i, len = strlen(DirName);
    if (DirName[len - 1] != '/') {
        strcat(DirName, "/");
        len = strlen(DirName);
    }

    for (i = 1; i < len; i++) {
        if (DirName[i] == '/') {
            DirName[i] = '\0';

            if (access(DirName, F_OK) == -1) {

                if (mkdir(DirName) == -1) {
                    perror("mkdir");
                    return -1;
                }
            }
            DirName[i] = '/';
        }
    }

    return 0;
}


void progress_bar(int n) {
	int i = 0;
	char bar[102];
	const char *lable = "|/-\\";
	bar[0] = 0;
	while (i < n) {
	    bar[i] = '#';
		i++;
		bar[i] = 0;
	}
	printf("\r[%-100s][%d%%][%c]", bar, i, lable[i%4]);
	fflush(stdout);
	return;
}



//Convert the input file into data and determine whether the file can be completely stored based on the size of the array.
int readRandomFileToDataArray(const string& file, querydata dataArray[], int arraySize) {
    fstream randomFile;
    randomFile.open(file);
    if (!randomFile.is_open()) {
        cerr << "Error in opening file: " << file << endl;
        return -1;
    }

    int datanum = 0;
    unsigned int a,b;
    while (randomFile >> a >> b) {
        if (datanum >= arraySize) {
            cout<<"datanum:"<<datanum<<"  arrsize"<<arraySize<<endl;
            cerr << "The number of input data exceeds the size of the array." << endl;
            break;
        }

        dataArray[datanum].node1 = a;
        dataArray[datanum].node2 = b;
        datanum++;
    }

    if (randomFile.fail() && !randomFile.eof()) {
        cerr << "Error reading file: " << file << endl;
    }

    randomFile.close();
    return datanum;//return query data number
}

//The `type` is selected according to the input data format.
int insert(Coral& coral, const string &filename, int type){
    unsigned int number=0;
    ifstream inputfile;
    inputfile.open(filename);
    cout<<"success open"<<filename<<endl;
    double alltime=0;
    timeval ts,te;



    if(type==0){
        string s1,s2;
        int w;
        while(inputfile>>s1>>s2){
            gettimeofday(&ts,NULL);
            coral.insert(s1,s2,1);
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            number++;
            if(number%1000000==0){
                cout<<"Coral had inserted "<<number<<" numbers datas"<<"   time is "<<alltime<<"useconds"<<endl;
            }
        }
    }

    if(type==1){
        string s1,s2;
        int w;
        string time;
        while(inputfile>>s1>>s2>>w>>time){
            gettimeofday(&ts,NULL);
            coral.insert(s1,s2,w);
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            number++;
            
            if(number%1000000==0){
                cout<<"Coral had inserted"<<number<<"numbers datas"<<"   time is"<<alltime<<"useconds"<<endl;
            }
        }
    }
    cout<<"over"<<endl;
    inputfile.close();
    return 0;

}

int insertinfo(Coral& coral, const string &filename, int type,string datainfopath){
    unsigned int number=0;
    ifstream inputfile;
    ofstream outputfile;
    inputfile.open(filename);
    outputfile.open(datainfopath);
    cout<<"insert file path: "<<filename<<endl;
    cout<<"result path"<<datainfopath<<endl;
    double alltime=0;
    double subtime=0;
    timeval ts,te;
    string middlefilepath="./Temple/";



    if(type==0){
        string s1,s2;
        int w;
        while(inputfile>>s1>>s2){
            gettimeofday(&ts,NULL);
            subtime+=coral.insertTime(s1,s2,1);
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            number++;
            if(number%1000000==0){
                cout<<"Coral had inserted "<<number<<" numbers datas"<<"   time is"<<(alltime-subtime)<<"useconds"<<endl;
                outputfile<<number<<"  "<<(alltime-subtime)<<endl;
            }
        }
    }

    if(type==1){
        string s1,s2;
        int w;
        string time;
        while(inputfile>>s1>>s2>>w>>time){
            gettimeofday(&ts,NULL);
            subtime+=coral.insertTime(s1,s2,w);
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            number++;            
            if(number%1000000==0){
                cout<<"Coral had inserted"<<number<<" numbers datas"<<"   time is"<<(alltime-subtime)<<"useconds"<<endl;
                outputfile<<number<<"  "<<(alltime-subtime)<<endl;
                

            }
        }
    }
    cout<<"over"<<endl;
    inputfile.close();
    return 0;    
}

unsigned int EdgeWeightQuery(Coral &coral,querydata dataArray[],int querysum){
    timeval ts,te;
    double sumtime=0;
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);
        gettimeofday(&ts,NULL);
        int res=coral.edgeWeightQuery(s1,s2);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    }
    cout<<"had query "<<querysum<<"data"<<endl;
    cout<<"query time "<<sumtime<<"usecond"<<endl;
    cout<<"averge query time "<<(sumtime/querysum)<<"usecond"<<endl;
    return 0;

}


unsigned int EdgeWeightQuery(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath){

    timeval ts,te;
    double sumtime=0;

    ofstream of(ResultFilePath);
    cout<<"out-put file path "<<ResultFilePath<<endl;
    
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);

        gettimeofday(&ts,NULL);
        int res=coral.edgeWeightQuery(s1,s2);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;

        of<<s1<<"  "<<s2<<"  "<<res<<endl;
    }

    

    cout<<"had query "<<querysum<<"data"<<endl;
    cout<<"query time "<<sumtime<<"usecond"<<endl;
    cout<<"averge query time "<<(sumtime/querysum)<<"usecond"<<endl;

    of.close();
    return 0;
}


unsigned int EdgeWeightQuery(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath,int times){

    timeval ts,te;
    timespec ts1, te1;
    

    ofstream of(ResultFilePath);
    cout<<"out-put file path "<<ResultFilePath<<endl;
    
    for(int i=0;i<querysum;i++){
        long long sumtime=0;
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);

        clock_gettime(CLOCK_MONOTONIC, &ts1);
        for(int j=0;j<times;j++){
            int res=coral.edgeWeightQuery(s1,s2);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te1);


         sumtime = timespec_diff(&ts1, &te1);

        of<<sumtime<<endl;
    }

    

    

    of.close();
    return 0;
}


unsigned int NodeQuery(Coral &coral,querydata dataArray[],int querysum,int type){

    timeval ts,te;
    double sumtime=0;
    
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);
    
        gettimeofday(&ts,NULL);
        int res1=coral.nodeWeightQuery(s1,type);
        int res2=coral.nodeWeightQuery(s2,type);

        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    }
    cout<<"this time query: "<<(2*querysum)<<"datas"<<endl;
    cout<<"tatol time:"<<sumtime<<"usecond"<<endl;
    cout<<"averge time: "<<(sumtime/(2*querysum))<<"usecond"<<endl;
    return 0;
}



unsigned int NodeQuery(Coral &coral,querydata dataArray[],int querysum,int type,string ResultFilePath){

    timeval ts,te;
    double sumtime=0;
    
    ofstream of(ResultFilePath);
    if(!of.is_open()){
        cout<<"Unable to open the file."<<endl;
    }

    
    for(int i=0;i<querysum;i++){

        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);

        gettimeofday(&ts,NULL);
        int res1=coral.nodeWeightQuery(s1,type);
        int res2=coral.nodeWeightQuery(s2,type);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;

        of<<s1<<" "<<res1<<" "<<s2<<" "<<res2<<endl;
    } 
    

    cout<<"this time query: "<<(2*querysum)<<"datas"<<endl;
    cout<<"tatol time:"<<sumtime<<"usecond"<<endl;
    cout<<"averge time: "<<(sumtime/(2*querysum))<<"usecond"<<endl;
    of.close();
    return 0;
}







unsigned int NodeQuery(Coral &coral,querydata dataArray[],int querysum,int type,string ResultFilePath,int times){

    timespec ts, te;
    
    
    
    ofstream of(ResultFilePath);
    if(!of.is_open()){
        cout<<"Unable to open the file."<<endl;
    }

    
    for(int i=0;i<querysum;i++){
        long long sumtime;

        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);

        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            int res1=coral.nodeWeightQuery(s1,type);
            int res2=coral.nodeWeightQuery(s2,type);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);

        sumtime = timespec_diff(&ts, &te);

        of<<sumtime/2<<endl;
    } 
    

    
    of.close();
    return 0;
}


unsigned int NodeDegreeQuery(Coral &coral,querydata dataArray[],int querysum,int type){

    timeval ts,te;
    double sumtime=0;
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);
        gettimeofday(&ts,NULL);
        int res1=coral.nodeDegreeQuery(s1,type);
        int res2=coral.nodeDegreeQuery(s2,type);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        cout<<"s1: "<<s1<<"res1:"<<res1<<"  s2: "<<s2<<"    res2:"<<res2<<endl;
    }
    cout<<"this times has query "<<(2*querysum)<<" datas"<<endl;
    cout<<"sumtimes is "<<sumtime<<" usecond"<<endl;
    return 0;



}


unsigned int NodeDegreeQuery(Coral &coral,querydata dataArray[],int querysum,int type,string ResultFilePath){

    timeval ts,te;
    double sumtime=0;
    
    ofstream of(ResultFilePath);
    if(!of.is_open()){
        cout<<"Unable to open the file."<<endl;
    }
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);
        gettimeofday(&ts,NULL);
        int res1=coral.nodeDegreeQuery(s1,type);
        int res2=coral.nodeDegreeQuery(s2,type);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        of<<s1<<"  "<<res1<<endl<<s2<<"  "<<res2<<endl;
    }
    cout<<"this time query: "<<(2*querysum)<<" datas"<<endl;
    cout<<"tatol time:"<<sumtime<<"usecond"<<endl;
    cout<<"averge time: "<<(sumtime/(2*querysum))<<"usecond"<<endl;
    of.close();
    return 0;

}


unsigned int NodeDegreeQuery(Coral &coral,querydata dataArray[],int querysum,int type,string ResultFilePath,int times){

    timeval ts,te;
    double sumtime=0;
    
    ofstream of(ResultFilePath);
    if(!of.is_open()){
        cout<<"Unable to open the file."<<endl;
    }
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);
        gettimeofday(&ts,NULL);
        for(int j=0;j<times;j++){
            int res1=coral.nodeDegreeQuery(s1,type);
            int res2=coral.nodeDegreeQuery(s2,type);
        }
        
        gettimeofday(&te,NULL);
        sumtime=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        of<<sumtime<<endl;
    }
    
    of.close();
    return 0;

}


unsigned int ReachabilityQuery(Coral &coral,querydata dataArray[],int querysum){
    timeval ts,te;
    double sumtime=0;
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);
        gettimeofday(&ts,NULL);
        bool res=coral.reachabilityQuery(s1,s2);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
    }
    cout<<"this time has queryed"<<querysum<<" datas"<<endl;
    cout<<"sumtime is "<<sumtime<<" usecond"<<endl;
    return 0;

}


unsigned int ReachabilityQuery(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath){
    timeval ts,te;
    double sumtime=0;

    
    ofstream of(ResultFilePath);
    for(int i=0;i<querysum;i++){
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);

        gettimeofday(&ts,NULL);
        bool res=coral.reachabilityQuery(s1,s2);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;

        of<<s1<<"  "<<s2<<"  "<<res<<endl;
    }
    

    
    cout<<"this time query: "<<querysum<<" datas"<<endl;
    cout<<"tatol time:"<<sumtime<<"usecond"<<endl;
    cout<<"averge time: "<<(sumtime/querysum)<<"usecond"<<endl;
    of.close();
    return 0;

}


unsigned int ReachabilityQuery(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath,int times){
    timespec ts, te;
    
    ofstream of(ResultFilePath);
    for(int i=0;i<querysum;i++){
        long long sumtime=0;
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);

        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            bool res=coral.reachabilityQuery(s1,s2);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
        of<<sumtime<<endl;
    }
    

    
   
    of.close();
    return 0;

}

unsigned int Findnearby(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath,int times){
    timespec ts, te;
    
    ofstream of(ResultFilePath);
    for(int i=0;i<querysum;i++){
        long long sumtime=0;
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);

        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            coral.findnearbyvertex(s1,3);
            coral.findnearbyvertex(s2,3);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
        of<<sumtime/2<<endl;
    }
    

    
   
    of.close();
    return 0;

}

unsigned int TopK(Coral &coral,querydata dataArray[],int querysum,string ResultFilePath,int times){
    timespec ts, te;
    
    ofstream of(ResultFilePath);
    vector<string> query{"459492","22908","103483","3976","1550284","925505","62801","732265"};
    for(int i=0;i<querysum;i++){
        long long sumtime=0;
        string s1=to_string(dataArray[i].node1);
        string s2=to_string(dataArray[i].node2);
        query.insert(query.end(),s1);
        query.insert(query.end(),s2);
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            coral.TopKheavyhitter(query);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
        query.pop_back();
        query.pop_back();

        of<<sumtime<<endl;
    }
    

    
   
    of.close();
    return 0;

}

#endif //QUERYFUNCTIONS__H
