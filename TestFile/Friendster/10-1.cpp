#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
	vector<int> v={20,40,60,80,100};
	string inputFilename = "friendsterqueryfile";
		
	string line;
	for(int num:v){
	int count=0;
	ifstream ifs(inputFilename+to_string(num));
	ofstream ofs(inputFilename+to_string(num)+"n");
	while(getline(ifs,line)){
	if(count%10==7){
	ofs<<line<<endl;
	}
	count++;
	}
	}
return 0;
	}
