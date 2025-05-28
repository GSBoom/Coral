#ifndef QUERYFUNCTION_H
#define QUERYFUNCTION_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
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
#include <unistd.h>
#include "GSS.h"
#include <time.h>


// const int query_data_pairs = 100000;
uint64_t query_data_pairs = 0;

struct QueryData {
	uint32_t a;
	uint32_t b;
};

long long timespec_diff(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000000000LL + (end->tv_nsec - start->tv_nsec);
}


/***************** function declaration ***********************/
// tool functions
int isFolderExist(char* folder);
int createDirectory(char* sPathName);
uint64_t count_lines(string file);
void progress_bar(int n);
int readRandomFileToDataArray(string file, QueryData dataArray[]);
// insert functions

// query functions

/***************** function declaration ***********************/

int isFolderExist(char* folder) {
	int ret = 0;
	ret = access(folder, R_OK | W_OK);
	if (ret == 0)
		ret = 1;
	else
		ret = 0;
	return ret;
}
int createDirectory(char* sPathName) {
	char DirName[256];
	strcpy(DirName, sPathName);
	int i, len = strlen(DirName);
	if (DirName[len - 1] != '/')
		strcat(DirName, "/");
	len = strlen(DirName);
	for (i = 1; i < len; i++) {
		if (DirName[i] == '/') {
			DirName[i] = 0;
			int a = access(DirName, F_OK);
			if (a == -1) {
				mkdir(DirName);
			}
			DirName[i] = '/';
		}
	}
	return 0;
}
// count lines of the input file
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


int readRandomFileToDataArray(string file, QueryData dataArray[]) {//将要查询的文件处理成可以直接进行
	ifstream randomFile;
	randomFile.open(file);
	if (!randomFile.is_open()) {
		cout << "Error in open file, Path = " << file << endl;
		return -1;
	}
	int datanum = 0;
	uint32_t a, b,c,d;
	while (!randomFile.eof()) {
		randomFile >> a >> b;
		if(randomFile.fail())
			break;
		dataArray[datanum].a = a;
		dataArray[datanum].b = b;
		datanum++;
	}
	randomFile.close();
	return datanum;
}


int insert(GSS &gss,const string &filename,int type){
    unsigned int number=0;
    ifstream inputfile;
    inputfile.open(filename);
    unsigned int datanum=0;
    timeval ts,te;
    double alltime=0;
    if(type==0){
        string s1,s2;
        while(inputfile>>s1>>s2){
            //cout<<"已经读取node1："<<s1<<"node2: "<<s2<<"w: "<<w<<endl;
            //if(s1=="114"&&s2=="135") gss.insertc(s1,s2,1);
            /*
            if(datanum==134177860){
                cout<<gss.edgeQueryc("114","135")<<endl;
                gss.insertc(s1,s2,1);
                cout<<gss.edgeQueryc("114","135")<<endl;
                cout<<gss.value[5570705].weight[0]<<endl;
            }
            */
            gettimeofday(&ts,NULL);
            gss.insert(s1,s2,1);
            gettimeofday(&te,NULL);
            
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            datanum++;
            //cout<<"成功插入第"<<datanum<<"条边"<<endl;
            if(datanum%1000000==0){
               cout<<datanum<<" "<<alltime<<" useconds"<<endl;
            }
        }
        
    }

    



    if(type==1){
        string s1,s2;
        int w;
        string time;
        while(inputfile>>s1>>s2>>w>>time){
            //cout<<"已经读取node1："<<s1<<"node2: "<<s2<<"w: "<<w<<endl;
            gettimeofday(&ts,NULL);
            gss.insert(s1,s2,w);
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            datanum++;
            //cout<<"成功插入第"<<datanum<<"条边"<<endl;
            if(datanum%1000000==0){
                cout<<datanum<<" "<<alltime<<" useconds"<<endl;
            }
        }

    }
    cout<<"data set volumn:"<<datanum<<endl;
    return 0;


}


int insert(GSS gss,const string &filename,int type,string outputfile){
	ofstream of(outputfile);
    unsigned int number=0;
    ifstream inputfile;
    inputfile.open(filename);
    unsigned int datanum=0;
    timeval ts,te;
    double alltime=0;
    if(type==1){
        string s1,s2;
        int w;
        string time;
        while(inputfile>>s1>>s2>>w>>time){
            //cout<<"已经读取node1："<<s1<<"node2: "<<s2<<"w: "<<w<<endl;
            gettimeofday(&ts,NULL);
            gss.insert(s1,s2,w);
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            datanum++;
            //cout<<"成功插入第"<<datanum<<"条边"<<endl;
            if(datanum%1000000==0){
                of<<datanum<<" "<<alltime<<endl;
            }
        }

    }

	if(type==0){
        string s1,s2;
        while(inputfile>>s1>>s2){
            //cout<<"已经读取node1："<<s1<<"node2: "<<s2<<"w: "<<w<<endl;
            gettimeofday(&ts,NULL);
            gss.insert(s1,s2,1);
            gettimeofday(&te,NULL);
            alltime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
            datanum++;
            //cout<<"成功插入第"<<datanum<<"条边"<<endl;
            if(datanum%1000000==0){
                of<<datanum<<" "<<alltime<<endl;
            }
        }

    }
    inputfile.close();
    return 0;


}


int edgeWeightQuery(GSS &gss,QueryData qdarray[],int querynum){

    timeval ts,te;
    double sumtime=0;

    for(int i=0;i<querynum;i++){
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        gettimeofday(&ts,NULL);
        int res=gss.edgeQuery(s1,s2);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
    }
    cout<<"this time has query "<<querynum<<" datas"<<endl;
    cout<<"total query time is "<<sumtime<<" usecond"<<endl;
	cout<<"average query time is "<<(sumtime/querynum)<<" usecond"<<endl;
    return 0;
}

int edgeWeightQuery(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath){
    timeval ts,te;
    double sumtime=0;
    ofstream of(resultfilepath);

    for(int i=0;i<querynum;i++){
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        gettimeofday(&ts,NULL);
        int res=gss.edgeQuery(s1,s2);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<s1<<" "<<s2<<" "<<res<<endl;
    }
    cout<<"this time has query "<<querynum<<" datas"<<endl;
    cout<<"total query time is "<<sumtime<<" usecond"<<endl;
	cout<<"average query time is "<<(sumtime/querynum)<<" usecond"<<endl;

    
    return 0;
}


int edgeWeightQuery(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath,int times){


    timespec ts, te;
    
    
    ofstream of(resultfilepath);

    for(int i=0;i<querynum;i++){
        long long sumtime=0;
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            int res=gss.edgeQuery(s1,s2);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        
        sumtime = timespec_diff(&ts, &te);
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<sumtime<<endl;
    }
    

    
    return 0;
}




int NodeQuery(GSS &gss,QueryData qdarray[],int querynum,int type)//type=0 outflow type=1 inflow
{
    timeval ts,te;
    double sumtime=0;

    for(int i=0;i<querynum;i++){
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        gettimeofday(&ts,NULL);
        int res1=gss.nodeValueQuery(s1,type);
        int res2=gss.nodeValueQuery(s2,type);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        //cout<<"s1: "<<s1<<"    res1:"<<res1<<" s2: "<<s2<<"    res2:"<<res2<<endl;
    }
    cout<<"this time has query "<<(2*querynum)<<" datas"<<endl;
    cout<<"total query time is "<<sumtime<<" usecond"<<endl;
	cout<<"average query time is "<<(sumtime/(2*querynum))<<" usecond"<<endl;
    return 0;
}


int NodeQuery(GSS &gss,QueryData qdarray[],int querynum,int type,string resultfile)//type=0 outflow type=1 inflow
{
    ofstream of(resultfile);
    timeval ts,te;
    double sumtime=0;

    for(int i=0;i<querynum;i++){
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);

        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        
        gettimeofday(&ts,NULL);
        int res1=gss.nodeValueQuery(s1,type);
        int res2=gss.nodeValueQuery(s2,type);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
       
        of<<s1<<" "<<res1<<" "<<s2<<" "<<res2<<endl;
    }
    
    cout<<"this time has query "<<(2*querynum)<<" datas"<<endl;
    cout<<"total query time is "<<sumtime<<" usecond"<<endl;
	cout<<"average query time is "<<(sumtime/(2*querynum))<<" usecond"<<endl;
    return 0;
}


int NodeQuery(GSS &gss,QueryData qdarray[],int querynum,int type,string resultfile,int times)//type=0 outflow type=1 inflow
{
    ofstream of(resultfile);
    timespec ts, te;
    

    for(int i=0;i<querynum;i++){
        long long sumtime=0;
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);

        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            int res1=gss.nodeValueQuery(s1,type);
            int res2=gss.nodeValueQuery(s2,type);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
       
        of<<sumtime/2<<endl;
    }
    
    
    return 0;
}



int reachabilityQuery(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath){
    timeval ts,te;
    double sumtime=0;
    ofstream of(resultfilepath);

    for(int i=0;i<querynum;i++){
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        gettimeofday(&ts,NULL);
        bool res=gss.reachabilityQuery(s1,s2);
        gettimeofday(&te,NULL);
        sumtime+=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<s1<<" "<<s2<<" "<<res<<endl;
    }
    cout<<"this time has query "<<querynum<<" datas"<<endl;
    cout<<"total query time is "<<sumtime<<" usecond"<<endl;
	cout<<"average query time is "<<(sumtime/querynum)<<" usecond"<<endl;

    
    return 0;
}

int reachabilityQuery(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath,int times){
    timespec ts, te;
    
    ofstream of(resultfilepath);

    for(int i=0;i<querynum;i++){
        long long sumtime=0;
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            bool res=gss.reachabilityQuery(s1,s2);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<sumtime<<endl;
    }
    

    
    return 0;
}

int nodeSuccessorQuery(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath,int times){
    timespec ts, te;
    
    ofstream of(resultfilepath);

    vector<string> res;
    for(int i=0;i<querynum;i++){
        long long sumtime=0;
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            gss.nodePrecursorQuery(s1,res);
            gss.nodePrecursorQuery(s2,res);

        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<sumtime/2<<endl;
    }
    

    
    return 0;
}

int nodePrecursorQuery(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath,int times){
    timespec ts, te;
    
    ofstream of(resultfilepath);

    vector<string> res;
    for(int i=0;i<querynum;i++){
        long long sumtime=0;
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            gss.nodeSuccessorQuery(s1,res);
            gss.nodeSuccessorQuery(s2,res);

        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<sumtime/2<<endl;
    }
    return 0;
}






int nearbyvertexk(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath,int times){
    timespec ts, te;
    
    ofstream of(resultfilepath);

    
    for(int i=0;i<querynum;i++){
        long long sumtime=0;
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            
            gss.findnearbyvertex(s1,3);
           
            gss.findnearbyvertex(s2,3);

        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        sumtime = timespec_diff(&ts, &te);
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<sumtime/2<<endl;
    }
    return 0;
}


int HeavyKQuery(GSS &gss,QueryData qdarray[],int querynum,string resultfilepath,int times){
    timespec ts, te;
    
    ofstream of(resultfilepath);
    vector<string> query={"459492","22908","103483","3976","1550284","925505","62801","732265"};
    vector<string> res;
    for(int i=0;i<querynum;i++){
        long long sumtime=0;
        string s1,s2;
        s1=to_string(qdarray[i].a);
        s2=to_string(qdarray[i].b);
        query.insert(query.end(),s1);
        query.insert(query.end(),s2);
        //cout<<"查询node1:"<<s1<<" node2:"<<s2<<endl;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for(int j=0;j<times;j++){
            gss.TopKheavyhitter(query);

        }
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        query.pop_back();
        query.pop_back();
        sumtime = timespec_diff(&ts, &te);
        //cout<<"s1: "<<s1<<"s2: "<<s2<<"    res:"<<res<<endl;
        of<<sumtime<<endl;
    }
    return 0;   
}
#endif // #ifndef QUERYFUNCTION_H