#include <iomanip>
#include "Scube/ScubeKick.h"
#include "headers/QueryFunction.h"
#include <windows.h>
#include <psapi.h>
#define DEBUG
#pragma comment(lib, "psapi.lib")
using namespace std;

// 链接 Psapi.lib 库


// 获取当前进程的内存使用信息
void GetProcessMemoryUsage(size_t& workingSetSize, size_t& privateBytes) {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
        workingSetSize = pmc.WorkingSetSize;
        privateBytes = pmc.PrivateUsage;
    }
}

int main(){

    size_t initialWorkingSet, initialPrivateBytes;
    GetProcessMemoryUsage(initialWorkingSet, initialPrivateBytes);

    string dataset_file_dir = ".\\DataSet\\";
    string data_set_name0="UK\\";
    string data_set_name1="dblp";
    string data_set_name=data_set_name0+data_set_name1;
    string dataset_file_path=dataset_file_dir+data_set_name;
    string queryinfo=".\\baselinecode\\Scube\\result\\";
    string query_data_dir=".\\TestFile\\";
    string path=queryinfo+data_set_name+"queryinfo";
    //vector<int> v={2500,5000,10000,20000,40000};//size of 
    vector<int> v={20,40,60,80,100};//size of 
    ofstream of(path); 


    int width, depth, fingerprint_length;	// Scube parameters
	double alpha = 0.8;						// Scube parameters
	uint32_t slot_nums, kick_times = 10;	// Scube parameters
	int ignore_bits = 8, reserved_bits = 2; // Scube parameters
	double exp_deg;							// Scube parameters

	string head_scube_addr = "";
	string head_degdetect_addr = "";
	string back_addr = "";
	int dataset = 2;						// specify the dataset for testing, (1) wiki-talk; (2) stackoverflow; (3) dbpedia; (4) caida.
	//string dataset_file_path;				// the dataset file path
	string input_dir;						// the inputfile dir path
	string output_dir;						// the outputfile dir path
	string dataset_name;					// the short name of the testing dataset
	int efflag = 0;							// the flag of edge frequence query
	int eeflag = 0;							// the flag of edge existence query
	int nfflag = 0;							// the flag of node frequence query
	int rpqflag = 0; 						// the flag of reachability path query
	int alledgeflag = 0;					// edge query for all unique edges
	uint32_t edge_n = 1000;					// repeat edge_n times for edge queries
	bool writeflag = false;					// write test results to a file
	int node_query_flag = 0;				// 1-node out query, 2-node in query
	int query_times = 1;					// query times
	int input_type = 0;
	int window = 10;
	int data_interval = 100;
	bool ins_bd = false;
	bool time_tp = false;
    slot_nums=100;
    exp_deg=200;
    width=2000;
    depth=2000;
    fingerprint_length=17;


    


    cout << "****************** Scube Initial *****************" << endl;
    Scube* scube = new ScubeKick(width, depth, fingerprint_length, kick_times, slot_nums, exp_deg, ignore_bits, reserved_bits, alpha);

    cout<<data_set_name<<endl;
    cout << "****************** Scube END *****************" <<endl;

    cout <<"****************** Scube insert start *****************" << endl;
    timeval ts,te;
    gettimeofday(&ts,NULL);
    insert(*scube, dataset_file_path, 0,10000);
    gettimeofday(&te,NULL);
    double alltime=(te.tv_sec-ts.tv_sec)*1000000+te.tv_usec-ts.tv_usec;
    cout<<"program running time"<<alltime<<"usecond"<<endl;
    cout <<"****************** Scube insert end *****************" << endl;
    cout<<endl;

    size_t finalWorkingSet, finalPrivateBytes;
    GetProcessMemoryUsage(finalWorkingSet, finalPrivateBytes);

    cout<<"data set name: "<<dataset_file_path<<" fingerprint: "<<fingerprint_length<<endl;
    cout<<"scube width: "<<width<<" scube room number: "<<ROOM<<endl;
    std::cout << "Initial Working Set: " << initialWorkingSet << " bytes" << std::endl;
    std::cout << "Final Working Set: " << finalWorkingSet << " bytes" << std::endl;
    std::cout << "Memory increase in Working Set: " << (finalWorkingSet - initialWorkingSet) << " bytes" << std::endl;

    std::cout << "Initial Private Bytes: " << initialPrivateBytes << " bytes" << std::endl;
    std::cout << "Final Private Bytes: " << finalPrivateBytes << " bytes" << std::endl;
    std::cout << "Memory increase in Private Bytes: " << (finalPrivateBytes - initialPrivateBytes) << " bytes" << std::endl;

  /* 
    for(int num:v){
        string queryfilepath=query_data_dir+data_set_name+"queryfile"+to_string(num);
        cout<<"input file name:"<<queryfilepath<<endl;

        int sizearray=count_lines(queryfilepath);
        cout<<"file lines:"<<sizearray<<endl; 
        QueryData* qdarray=new QueryData[sizearray+10];
        int querynum=readRandomFileToDataArray(queryfilepath,qdarray,sizearray+10);


        
        cout <<"****************** Scube edge Weight Query start *****************" << endl;
        string result_path=queryinfo+data_set_name0+to_string(num)+"edge_weight_info.txt";
        cout<<"result file path "<<result_path<<endl;
        edgeFrequenceQueryTest(*scube,qdarray,querynum,1,result_path); 
        cout <<"****************** Scube edge Weight Query end *****************" << endl;
        cout<<endl;

        cout <<"****************** Scube inflow-Nodequery start *****************" << endl;
        string result_path1=queryinfo+data_set_name0+to_string(num)+"inflow_weight_info.txt";
        nodeFrequenceQueryTest(*scube,qdarray,querynum,1,result_path1,1);
        cout <<"****************** Scube inflow-Nodequery end *****************" << endl;
        cout<<endl;

        cout <<"****************** Scube outflow-Nodequery start *****************" << endl;
        string result_path2=queryinfo+data_set_name0+to_string(num)+"outflow_weight_info.txt";
        nodeFrequenceQueryTest(*scube,qdarray,querynum,1,result_path2,0);
        cout <<"****************** Scube outflow-Nodequery end *****************" << endl;
        cout<<endl;

        cout <<"****************** Scube reachability query start *****************" << endl;
        string result_path3=queryinfo+data_set_name0+to_string(num)+"reachability_info.txt";
        reachabilityQueryTest(*scube,qdarray,querynum,1,result_path3);
        cout <<"****************** Scube reachability query end *****************" << endl;
        cout<<endl;

        delete[] qdarray;
        
    }*/ 
return 0;
}