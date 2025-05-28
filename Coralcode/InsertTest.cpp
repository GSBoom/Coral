#include <iostream>
#include <fstream>
#include <string>
#include "queryfunctions.h"
#include "Coral.h"
#include <cstdint>
#include <cstring>
#include <ctime>

using namespace std;

int main(){


    string dataset_file_dir = "./DataSet/";
    string data_set_name="USA-Road/usa-road";
    string dataset_file_path = dataset_file_dir + data_set_name;

    string result_dir="./ResultNew/InsertTest/";

    cout << "****************** CORAL Initial *****************" << endl;
    Coral coral(3000 , 16,1000,0.01,16,4);//Coral(int width, int fingerprint_length,int maxhotrownumber,double falsePositiveRate,int comparenumber,int hotrowtimes)
    cout<<"basket matrix width:"<<coral.widthpublic<<endl;
    cout<<"coral cell Matrix Width:"<<MatrixWidth<<endl;
    cout <<"****************** CORAL insert start *****************" << endl;
    string result_path=result_dir+data_set_name+to_string(coral.widthpublic)+to_string(MatrixWidth);
    cout<<"result path: "<<result_path<<endl;
    insertinfo(coral, dataset_file_path,0,result_path);//Save the result file 
    coral.printbasicparameter();
    cout <<"****************** CORAL insert end *****************" << endl;
    cout<<endl;


    return 0;
}