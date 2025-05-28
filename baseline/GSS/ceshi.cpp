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

int main(){
    GSS gss(4000,4,4,16,16,false,0);
    gss.insert("114","135",2);
    cout<<" query s1:119 s2:116 "<<gss.edgeQuery("114","135");
    return 0;
}