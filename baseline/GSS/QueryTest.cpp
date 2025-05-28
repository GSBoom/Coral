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
    vector<int> v={20,40,60,80,100};//size of 
    
    cout << "****************** GSS Initial *****************" << endl;
    GSS gss(2000,4,4,4,16,false,0);//GSS(int width, int range, int p_num, int size,int f_num, bool usehashtable, int TableSize)
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
    

    for(int num:v){
        string queryfilepath=query_data_dir+data_set_name+"queryfile"+to_string(num);
        cout<<"input file name:"<<queryfilepath<<endl;
        
        int sizearray=count_lines(queryfilepath);
        cout<<"file lines:"<<sizearray<<endl;  
        QueryData* qdarray=new QueryData[sizearray+10];  
        int querynum=readRandomFileToDataArray(queryfilepath,qdarray);


/*
        cout <<"****************** GSS edge Weight Query start *****************" << endl;
        string result_path=queryinfo+data_set_name0+to_string(num)+gssparameter+"edge_weight_info.txt";
        edgeWeightQuery(gss,qdarray,querynum,result_path); 
        cout <<"****************** GSS edge Weight Query end *****************" << endl; 
        cout<<endl;

*/

        cout <<"****************** GSS inflow-Nodequery start *****************" << endl;
        string result_path1=queryinfo+data_set_name0+to_string(num)+gssparameter+"inflow_node_info.txt";
        //NodeQuery(gss,qdarray,querynum,1);
        NodeQuery(gss,qdarray,querynum,1,result_path1);
        cout <<"****************** GSS inflow-Nodequery end *****************" << endl;
        cout<<endl;

/*
        cout <<"****************** GSS outflow-Nodequery start *****************" << endl;
        string result_path2=queryinfo+data_set_name0+to_string(num)+gssparameter+"outflow_weight_info.txt";
        NodeQuery(gss,qdarray,querynum,0,result_path2);
        cout <<"****************** GSS outflow-Nodequery end *****************" << endl;
        cout<<endl;


        cout <<"****************** GSS reachability query start *****************" << endl;
        string result_path3=queryinfo+data_set_name0+to_string(num)+gssparameter+"reachability_info.txt";
        reachabilityQuery(gss,qdarray,querynum,result_path3);
        cout <<"****************** GSS reachability query end *****************" << endl;
        cout<<endl;
        */
    }


   
    /*
   */

     
    /*
    
    */    



    
    
        



   


    return 0;
}