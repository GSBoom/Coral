#include <iostream>
#include <fstream>
#include <string>
#include "queryfunctions.h"
#include "Coral.h"
#include <cstdint>
#include <cstring>
#include <ctime>

using namespace std;


int main()
{

    string path;
    string dataset_file_path = "./DataSet/DBLP/dblp";
    ofstream of(path); 
    cout << "****************** CORAL Initial *****************" << endl;
    Coral coral(2000, 16,1000,0.01,18,4);
    cout<<"basket matrix width:"<<coral.widthpublic<<endl;
    cout<<"coral cell Matrix Width:"<<MatrixWidth<<endl;
    cout <<"****************** CORAL insert start *****************" << endl;
    timeval ts,te;
    gettimeofday(&ts,NULL);
    insert(coral, dataset_file_path, 1);
    gettimeofday(&te,NULL);
    double alltime=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    
    cout<<"program running time"<<alltime<<"usecond"<<endl;
    cout <<"****************** CORAL insert end *****************" << endl;
    cout<<endl;
    
    
    int res=coral.findnearbyvertex("2",3);
    cout<<"result findnearbyvertex:"<<res<<endl;
    cout<<" over"<<endl;

    return 0;
}

