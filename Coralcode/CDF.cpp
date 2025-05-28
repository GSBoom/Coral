#include <iostream>
#include <fstream>
#include <string>
#include "queryfunctions.h"
#include "Coral.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include <vector>

//This file is used for the CDF (Cumulative Distribution Function) experiment. The generated file records the time of each query, with the unit being nanoseconds.

using namespace std;


int main()
{

    
    string dataset_file_dir = ".\\DataSet\\";
    string data_set_name0="UK-2002\\";
    string data_set_name1="uk-2002";
    string data_set_name=data_set_name0+data_set_name1;
    string dataset_file_path=dataset_file_dir+data_set_name;
    string queryinfo=".\\ResultNew\\CDF\\";
    string query_data_dir=".\\TestFile\\";
    string path=queryinfo+data_set_name+"queryinfo";
    vector<int> v={1};
    ofstream of(path); 
    cout << "****************** CORAL Initial *****************" << endl;
    Coral coral(4000, 16,1000,0.01,18,4);
    string coralparameter="-"+to_string(coral.widthpublic)+"-"+to_string(MatrixWidth)+"-";
    cout<<data_set_name<<endl;
    cout<<"basket matrix width:"<<coral.widthpublic<<endl;
    cout<<"coral cell Matrix Width:"<<MatrixWidth<<endl;
    cout << "****************** CORAL END *****************" << endl;

    cout <<"****************** CORAL insert start *****************" << endl;
    timeval ts,te;
    gettimeofday(&ts,NULL);
    insert(coral, dataset_file_path, 0);
    gettimeofday(&te,NULL);
    double alltime=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    cout<<"program running time"<<alltime<<"usecond"<<endl;
    cout <<"****************** CORAL insert end *****************" << endl;
    cout<<endl;
    
        string queryfilepath=query_data_dir+data_set_name+"queryfile"+"cdf";
        cout<<"input file name:"<<queryfilepath<<endl;

        int sizearray=count_lines(queryfilepath);
        cout<<"file lines:"<<sizearray<<endl; 
        querydata* qdarray=new querydata[sizearray+10];
        int querynum=readRandomFileToDataArray(queryfilepath,qdarray,sizearray+10);



        cout <<"****************** CORAL TopK Query start *****************" << endl;
        string result_path5=queryinfo+data_set_name0+"cdf"+coralparameter+"topk.txt";
        TopK(coral,qdarray,querynum,result_path5,1); 
         
        cout <<"****************** CORAL TopK Query end *****************" << endl;
        cout<<endl;



        cout <<"****************** CORAL Nearby node Query start *****************" << endl;
        string result_path6=queryinfo+data_set_name0+"cdf"+coralparameter+"nearby.txt";
        Findnearby(coral,qdarray,querynum,result_path6,1); 
        
        cout <<"****************** CORAL Nearby node Query end *****************" << endl;
        cout<<endl;
        

        
        cout <<"****************** CORAL edge Weight Query start *****************" << endl;
        string result_path=queryinfo+data_set_name0+"cdf"+coralparameter+"1edge_weight_info.txt";
        EdgeWeightQuery(coral,qdarray,querynum,result_path,1); 
        //EdgeWeightQuery(coral,qdarray,querynum,result_path); 
        cout <<"****************** CORAL edge Weight Query end *****************" << endl;
        cout<<endl;

 
        cout <<"****************** CORAL inflow-Nodequery start *****************" << endl;
        string result_path1=queryinfo+data_set_name0+"cdf"+coralparameter+"1inflow_weight_info.txt";
        NodeQuery(coral,qdarray,querynum,1,result_path1,1);
        //NodeQuery(coral,qdarray,querynum,1,result_path1);
        cout <<"****************** CORAL inflow-Nodequery end *****************" << endl;
        cout<<endl;

        cout <<"****************** CORAL outflow-Nodequery start *****************" << endl;
        string result_path2=queryinfo+data_set_name0+"cdf"+coralparameter+"1outflow_weight_info.txt";
        NodeQuery(coral,qdarray,querynum,0,result_path2,1);
        //NodeQuery(coral,qdarray,querynum,0,result_path2);
        cout <<"****************** CORAL outflow-Nodequery end *****************" << endl;
        cout<<endl;

        cout <<"****************** CORAL reachability query start *****************" << endl;
        string result_path3=queryinfo+data_set_name0+"cdf"+coralparameter+"1reachability_info.txt";
        ReachabilityQuery(coral,qdarray,querynum,result_path3,1);
        //ReachabilityQuery(coral,qdarray,querynum,result_path3);
        cout <<"****************** CORAL reachability query end *****************" << endl;

        cout<<endl;

        delete[] qdarray;



    
    




   


    return 0;
}