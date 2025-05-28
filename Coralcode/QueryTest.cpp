#include <iostream>
#include <fstream>
#include <string>
#include "queryfunctions.h"
#include "Coral.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include <vector>


using namespace std;


int main()
{

    
    string dataset_file_dir = ".\\DataSet\\";
    string data_set_name0="USA-Road\\";
    string data_set_name1="usa-road";
    string data_set_name=data_set_name0+data_set_name1;
    string dataset_file_path=dataset_file_dir+data_set_name;
    string queryinfo=".\\ResultNew\\QueryTest\\";
    string query_data_dir=".\\TestFile\\";
    string path=queryinfo+data_set_name+"queryinfo";
    string insertresultpath;
    //vector<int> v={2500,5000,10000,20000,40000};//size of 
    vector<int> v={20,40,60,80,100};//size of 
    ofstream of(path); 
    cout << "****************** CORAL Initial *****************" << endl;
    Coral coral(2500, 16,1000,0.01,18,4);
    string coralparameter="-"+to_string(coral.widthpublic)+"-"+to_string(MatrixWidth)+"-";
    cout<<data_set_name<<endl;
    cout<<"basket matrix width:"<<coral.widthpublic<<endl;
    cout<<"coral cell Matrix Width:"<<MatrixWidth<<endl;
    cout << "****************** CORAL END *****************" << endl;

    cout <<"****************** CORAL insert start *****************" << endl;
    timeval ts,te;
    gettimeofday(&ts,NULL);
    insertinfo(coral, dataset_file_path, 0,insertresultpath);
    gettimeofday(&te,NULL);
    double alltime=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    cout<<"program running time"<<alltime<<"usecond"<<endl;
    cout <<"****************** CORAL insert end *****************" << endl;
    cout<<endl;
    for(int num:v){
        string queryfilepath=query_data_dir+data_set_name+"queryfile"+to_string(num);
        cout<<"input file name:"<<queryfilepath<<endl;

        int sizearray=count_lines(queryfilepath);
        cout<<"file lines:"<<sizearray<<endl; 
        querydata* qdarray=new querydata[sizearray+10];
        int querynum=readRandomFileToDataArray(queryfilepath,qdarray,sizearray+10);


        
        cout <<"****************** CORAL edge Weight Query start *****************" << endl;
        string result_path=queryinfo+data_set_name0+to_string(num)+coralparameter+"edge_weight_info.txt";
        EdgeWeightQuery(coral,qdarray,querynum,result_path); 
        cout <<"****************** CORAL edge Weight Query end *****************" << endl;
        cout<<endl;

 
        cout <<"****************** CORAL inflow-Nodequery start *****************" << endl;
        string result_path1=queryinfo+data_set_name0+to_string(num)+coralparameter+"inflow_weight_info.txt";
        NodeQuery(coral,qdarray,querynum,1,result_path1);
        cout <<"****************** CORAL inflow-Nodequery end *****************" << endl;
        cout<<endl;

        cout <<"****************** CORAL outflow-Nodequery start *****************" << endl;
        string result_path2=queryinfo+data_set_name0+to_string(num)+coralparameter+"outflow_weight_info.txt";
        NodeQuery(coral,qdarray,querynum,0,result_path2);
        cout <<"****************** CORAL outflow-Nodequery end *****************" << endl;
        cout<<endl;

        cout <<"****************** CORAL reachability query start *****************" << endl;
        string result_path3=queryinfo+data_set_name0+to_string(num)+coralparameter+"reachability_info.txt";
        ReachabilityQuery(coral,qdarray,querynum,result_path3);
        cout <<"****************** CORAL reachability query end *****************" << endl;
        cout<<endl;

        delete[] qdarray;



    }
    




   


    return 0;
}