#include <iostream>
#include <fstream>
#include <cstdlib>  // rand() and srand() functions
#include <ctime>    // time() functions
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>
#include "queryfunctions.h"


using namespace std;

//get random file



int main()
{
    //vector<int> v{2500,5000,10000,20000,40000};
    vector<int> partition{20,40,60,80,100};
    string oralfiledir=".\\DataSet\\";
    string datasetname="Delicious-ui\\delicious-ui";
    string oralfilepath=oralfiledir+datasetname;
    cout<<"oral path: "<<oralfilepath<<endl;

    string rfilequery;
    rfilequery=".\\TestFile\\"+datasetname+"queryfile";
    cout<<"queryfile path: "<<rfilequery<<endl;

    ifstream ifs;
    string line;
    string s1,s2;
    int w;
    string t;
    uint64_t filelenth=count_lines(oralfilepath);

    ifs.open(oralfilepath);
    int dis=filelenth/10000;
    cout<<"dis:"<<dis<<endl;
    int random=generateRandomNumber(dis);
    string nrfilequery=rfilequery+"cdf";
    ofstream ofs;
    ofs.open(nrfilequery);
    int number=0;
    while(ifs>>s1>>s2>>w>>t){
        number++;
        if((number-random)%dis==0){
            ofs<<s1<<" "<<s2<<endl;
        }
    }

    /*
    for(int num:partition){
        ifs.open(oralfilepath);
        uint64_t readnowlinenum=filelenth*num/100;
        unsigned int numno=0;
        string nrfilequery=rfilequery+to_string(num);
        ofstream ofs;
        ofs.open(nrfilequery);
        int random=generateRandomNumber(1000);
        cout<<"random:"<<random<<endl;
        while(ifs>>s1>>s2&&numno<=readnowlinenum){
            if((numno-random)%1000==0){
            ofs<<s1<<" "<<s2<<endl;
            //cout<<"insert one line"<<line;
            }
            numno++;
        }

        ofs.close();
        ifs.close();
    }

*/
    return 0;
}
