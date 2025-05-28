#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    string line;
    ifstream ifs(".\\DataSet\\Friendster\\friendster");
    if (!ifs.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }
   
    getline(ifs,line);
    cout<<line<<endl;
    getline(ifs,line);
    cout<<line<<endl;
    getline(ifs,line);
    cout<<line<<endl;
    getline(ifs,line);
    cout<<line<<endl;
    getline(ifs,line);
    cout<<line<<endl;
    getline(ifs,line);
    cout<<line<<endl;
    getline(ifs,line);
    cout<<line<<endl;

    cout<<line<<endl;
    return 0;
}