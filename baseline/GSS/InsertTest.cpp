#include <iostream>
#include <fstream>
#include <string>
#include "queryfunctions.h"
#include "GSS.h"
#include <cstdint>
#include <cstring>
#include <ctime>

using namespace std;


int main()
{

    string path;
    string dataset_file_dir = ".\\DataSet\\";
    string data_set_name="USA-Road\\usa-road";
    string dataset_file_path=dataset_file_dir+data_set_name;
    string query_data_dir=".\\TestFile\\";
    string result_dir=".\\baselinecode\\GSS\\Result\\InsertTest\\";
    ofstream of(path); 
    basket a;
    linknode b;
    cout << "****************** GSS Initial *****************" << endl;
    GSS gss(2500,1,1,4,17,false,0);//矩阵宽度 没有用还是只用了一个hash函数 一个位置 4为roomnum 16为指纹的位数 不用hashtable 后面是如果用hashtable的长度
    
    cout <<"****************** GSS insert start *****************" << endl;
    string result_path=result_dir+data_set_name+to_string(gss.w)+to_string(gss.s);
    timeval ts,te;
    gettimeofday(&ts,NULL);
    cout<<dataset_file_path<<endl;
    insert(gss, dataset_file_path, 0);
    gettimeofday(&te,NULL);
    double alltime=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    double utilization=static_cast<double>(gss.differentedge)/(gss.w*gss.w*Roomnum+gss.datainbuffer);
    double utilizationinmatrix=static_cast<double>(gss.differentedge-gss.datainbuffer)/(gss.w*gss.w*Roomnum);
    cout<<"different edge number :"<<gss.differentedge<<endl;
    cout<<"utilized: "<<utilization<<" utilized matrix: "<<utilizationinmatrix<<endl;
    cout<<"gss's width: "<<gss.w<<" gss's Roomnum: "<<Roomnum<<endl;
    cout<<"gss insert program running time "<<alltime<<" usecond"<<endl;
    cout<<"inserted data set :"<<data_set_name<<endl;
    cout<<"fingerprint bits:"<<gss.f<<endl;
    cout<<"basket memory cost: "<<sizeof(a)<<" node of buffer memory cost: "<<sizeof(b)<<endl;
    cout<<"Size of This GSS matrix " << gss.w*gss.w*sizeof(a) << " bytes" << endl;
    cout<<"number datas in buffer: "<<gss.datainbuffer<<" size of buffer "<<gss.datainbuffer*sizeof(b)<<endl;
    cout<<"memory cost for store :"<<gss.w*gss.w*sizeof(a)+gss.datainbuffer*sizeof(b)<<endl;
    cout <<"****************** GSS insert end *****************" << endl;
    cout<<endl;
    
    /*
    int num=5000;
    string queryfilepath=query_data_dir+data_set_name+"queryfile"+to_string(num);
    cout<<"input file name:"<<queryfilepath<<endl;
    int sizearray=count_lines(queryfilepath);
    cout<<"sizearry:"<<sizearray<<endl;
    QueryData* qdarray=new QueryData[sizearray+10];
    int querynum=readRandomFileToDataArray(queryfilepath,qdarray);*/

    //cout <<"****************** GSS edge query start *****************" << endl;
    //edgeWeightQuery(gss,qdarray,querynum);
    //cout <<"****************** GSS edge query end *****************" << endl;
    return 0;

}

