#include <iostream>
#include <fstream>
#include <string>
#include "queryfunctions.h"
#include "GSS.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include <vector>


using namespace std;


int main()
{

    
    string dataset_file_dir = ".\\DataSet\\";
    string data_set_name0="DBLP\\";
    string data_set_name1="dblp";
    string data_set_name=data_set_name0+data_set_name1;
    string dataset_file_path=dataset_file_dir+data_set_name;
    string queryinfo=".\\baselinecode\\GSS\\Result\\QueryTest\\";
    string query_data_dir=".\\TestFile\\";
    string path=queryinfo+data_set_name+"queryinfo";
    //vector<int> v={2500,5000,10000,20000,40000};//size of 
    //vector<int> v={20,40,60,80,100};//size of 
    vector<int> v={20};//size of 
    cout << "****************** GSS Initial *****************" << endl;
    GSS gss(2000,4,4,16,16,false,0);//GSS(int width, int range, int p_num, int size,int f_num, bool usehashtable, int TableSize)
    string gssparameter="-"+to_string(gss.w)+"-"+to_string(Roomnum)+"-";
    cout<<data_set_name<<endl;
    cout<<"basket matrix width:"<<gss.w<<endl;
    cout<<"gss room number:"<<gss.s<<endl;
    cout <<"****************** GSS insert start *****************" << endl;
    timeval ts,te;
    gettimeofday(&ts,NULL);
    insert(gss, dataset_file_path, 1);//the last parameter decided by input data struct
    gettimeofday(&te,NULL);
    double alltime=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    cout<<"program running time"<<alltime<<"usecond"<<endl;
    cout <<"****************** GSS insert end *****************" << endl;

    cout<<endl;
    cout<<gss.findnearbyvertex("65",3);
}