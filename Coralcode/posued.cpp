#include <iostream>
#include <fstream>
#include <string>
#include "queryfunctions.h"
#include "Coral.h"
#include <cstdint>
#include <cstring>
#include <ctime>

using namespace std;

//this program can save bucket usage
int main(){
    Coral coral(2000 , 16,1000,0.01,16,4);
    string dataset_file_dir = ".\\DataSet\\";
    string data_set_name="DBLP\\dblp";
    string dataset_file_path=dataset_file_dir+data_set_name;
    insert(coral, dataset_file_path, 1);

    string outputdir=".\\Result\\BasketUse\\";
    string outfileparameter="-1500-4";
    string overfix=".txt";
    string out_file1=outputdir+data_set_name+outfileparameter+"-oralpos"+overfix;
    string out_file2=outputdir+data_set_name+outfileparameter+"-coralpos"+overfix;
    string out_file3=outputdir+data_set_name+outfileparameter+"-kinds"+overfix;
    string out_file4=outputdir+data_set_name+outfileparameter+"-corallocaluse"+overfix;
    string out_file5=outputdir+data_set_name+outfileparameter+"-coralglobaluse"+overfix;

    coral.printoralused(out_file1);
    coral.printavailable(out_file2);
    coral.printkinds(out_file3);
    coral.printlocaluse(out_file4);
    coral.printglobaluse(out_file5);

    
    for(int num:coral.value[0].next){
        cout<<num<<"  ";
    }
    for(int i=0;i<coral.widthpublic*coral.widthpublic;i++){
        if(coral.value[i].next.size()>2)
            cout<<" pos:"<<i<<" "<<coral.value[i].next.size();
        }

    return 0;
    
    



}