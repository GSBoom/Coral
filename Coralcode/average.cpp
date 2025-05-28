#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

int main() {
    // here is a file include number
    string inputfilepath="baselinecode\\GSS\\Result\\QueryTest\\UK-2002\\cdfnearby.txt";
    std::ifstream inputFile(inputfilepath);
    if (!inputFile.is_open()) {
        std::cerr << "open failed" << std::endl;
        return 1;
    }

    double sumtime=0;
    int num=0;
    std::vector<int> numbers;
    int number;
    // readnumber
    while (inputFile >> number) {
        num++;
        sumtime+=number/1000;//get average number /100  in our methods ns->us 
    }
    cout<<inputfilepath<<"average time(us):"<<sumtime/num<<endl;
    inputFile.close();

    
    return 0;
}