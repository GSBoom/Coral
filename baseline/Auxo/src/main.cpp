#include "querysupportstruct.h"
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <set>
#include <time.h>

using namespace std;

long long timespec_diff(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000000000LL + (end->tv_nsec - start->tv_nsec);
}

int main(int argc, char* argv[])
{
    string dataset="../../data/lkml/rawdata.txt";
    string nodeOut="../../data/lkml/nodeOutQueryTest.txt";
    string nodeIn="../../data/lkml/nodeInQueryTest.txt";
    string edge="../../data/lkml/edgeQueryTest.txt";
    string result="../result/usa-query.txt";
    string queryfile="..\\..\\..\\TestFile\\UK-2002\\uK-2002queryfilecdf";
    //vector<int> v={20,40,60,80,100};
    vector<int> v={20};

    int fpl=16,cols=4,candiNum=16,width=100;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-queryfile") == 0) {
			queryfile = argv[++i];
		}

		if (strcmp(argv[i], "-dataset") == 0) {
			dataset = argv[++i];
		}
		if (strcmp(argv[i], "-result") == 0) {
			result = argv[++i];
		}
		if (strcmp(argv[i], "-fpl") == 0) {
			fpl = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-width") == 0) {
			width = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-cols") == 0) {
			cols = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-candiNum") == 0) {
			candiNum = atoi(argv[++i]);
		}
    }
    cout<<"The parameters are shown as below:\n"<<"dataset: "<<dataset<<" result: "<<result<<" cols: "<<cols<<" candiNum: "<<candiNum<<" fpl: "<<fpl<<endl;

    Auxo *auxo=new Auxo(width,cols,candiNum,fpl);//side width of the matrix, length of the hash address, number of the candidate bucket, length of the fingerprint
    AuxoPro *auxopro=new AuxoPro(width,cols,candiNum,fpl);
    GSS_Chain *gsschain=new GSS_Chain(width,cols,candiNum,fpl);
    int num=0;

    ofstream out1(result);

    ifstream ifs;
    ifs.open(dataset);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << dataset << endl;
		return -1;
	}

	int64_t s, d;
    int w;
    unsigned int t;
    double AuxoMatrixTime=0,
           ComMatrixTime=0,
           GSSMatrixTime=0,
           migrationtime=0;
    unsigned int minumber=0;
    cout<<"Insertion starts\n";
    while(!ifs.eof())
    {
        ifs >> s >> d>>w>>t;
        //out1<<s<<" "<<d<<" "<<w<<" "<<t<<" ";
        //auxo->insert(to_string(s), to_string(d),w,AuxoMatrixTime);
        //auxopro->insert(to_string(s), to_string(d),w,ComMatrixTime);
        auxo->inserttest(to_string(s), to_string(d),1,AuxoMatrixTime,minumber,migrationtime);
        auxopro->insert(to_string(s), to_string(d),1,ComMatrixTime);
        //gsschain->insert(to_string(s), to_string(d),w,GSSMatrixTime);
        num++;
        if(num%1000000==0){
            out1<<"Auxo has inserted "<<num<<" datas "<<"insert time is "<<AuxoMatrixTime<<" useconds"<<endl;
            out1<<"Auxopro has inserted "<<num<<" datas "<<"insert time is "<<ComMatrixTime<<" useconds"<<endl;
            //out1<<"Auxo has inserted "<<num<<" datas "<<"insert time is "<<GSSMatrixTime<<" useconds"<<endl;
        }
    }
    /*
    out1<<"migration times: "<<minumber<<" migrationtime: "<<migrationtime<<endl;
    out1<<"The over all throughput of Auxo: "<<num/(AuxoMatrixTime/1e6)<<"/s\n";
    out1<<"The over all throughput of proportional Auxo: "<<num/(ComMatrixTime/1e6)<<"/s\n";
    out1<<"total insert "<<num<<" datas, Auxo use "<<AuxoMatrixTime<<" useconds"<<endl;
    out1<<"total insert "<<num<<" datas, Auxo-pro use"<<ComMatrixTime<<" useconds"<<endl;
    //out1<<"The over all throughput of GSS_Chain: "<<num/(GSSMatrixTime/1e6)<<"/s\n";
    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<"The consumed memory of fingerprints for Auxo: "<<(double)(2*auxo->memoryAllocated2)/131072<<" M\n";
    out1<<"The consumed memory of fingerprints for proportional Auxo: "<<(double)(2*auxopro->memoryAllocated2)/131072<<" M\n";
    //out1<<"The consumed memory of fingerprints for GSS_Chain: "<<(double)(gsschain->memoryAllocated2)/131072<<" M\n";
    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<endl<<endl;
    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<"The consumed memory of weight for Auxo: "<<(double)(auxo->memoryAllocated1)/131072<<" M\n";
    out1<<"The consumed memory of weight for proportional Auxo: "<<(double)(auxopro->memoryAllocated1)/131072<<" M\n";
    //out1<<"The consumed memory of fingerprints for GSS_Chain: "<<(double)(gsschain->memoryAllocated2)/131072<<" M\n";
    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<endl<<endl;
    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<"The consumed memory of index for Auxo: "<<(double)(auxo->memoryAllocated3)/131072<<" M\n";
    out1<<"The consumed memory of index for proportional Auxo: "<<(double)(auxopro->memoryAllocated3)/131072<<" M\n";
    //out1<<"The consumed memory of fingerprints for GSS_Chain: "<<(double)(gsschain->memoryAllocated2)/131072<<" M\n";
    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<endl<<endl;

    out1<<"\n-----------------------------------------------------------------------------------\n\n";
    out1<<"The consumed memory Auxo: "<<(double)(auxo->memoryAllocated1+2*auxo->memoryAllocated2+auxo->memoryAllocated3)/131072<<" M\n";
    out1<<"The consumed memory proportional Auxo: "<<(double)(auxopro->memoryAllocated1+2*auxopro->memoryAllocated2+auxopro->memoryAllocated3)/131072<<" M\n";
    //out1<<"The consumed memory of fingerprints for GSS_Chain: "<<(double)(gsschain->memoryAllocated2)/131072<<" M\n";
    out1<<"\n-----------------------------------------------------------------------------------\n\n";

    unsigned int count=0;
    ofstream ofedge("..//result//dblpcdfedge");
    ofstream ofin("..//result//dblpcdfin");
    ofstream ofout("..//result//dblpcdfout");
    ofstream ofreach("..//result//dblpcdfreach");

    for(int num:v){
        string queryfilepath=queryfile;
        ifstream ifs1;
        double edgequerytime=0;
        double nodeinflowtime=0;
        double nodeoutflowtime=0;
        double reachabilitytime=0;

        double edgequeryprotime=0;
        double nodeinflowprotime=0;
        double nodeoutflowprotime=0;
        double reachabilityprotime=0;

        
        timespec ts, te;
        ifs1.open(queryfilepath);
	    if (!ifs1.is_open()) {
            cout << "Error in open file, Path = " << queryfilepath << endl;
		    return -1;
        
        }
        out1<<"queryfile now is "<<queryfilepath<<endl;

        while(!ifs1.eof()){
            string a,b;
            ifs1>>a>>b;
            count++;
            long long sumtime1=0;
            long long sumtime2=0;
            long long sumtime3=0;
            long long sumtime4=0;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            auxo->edgeQuery(a,b,edgequerytime);
            clock_gettime(CLOCK_MONOTONIC, &te);
            //sumtime1 = timespec_diff(&ts, &te);
            ofedge<<sumtime1<<endl;
            auxopro->edgeQuery(a,b,edgequeryprotime);
            //
            clock_gettime(CLOCK_MONOTONIC, &ts);
            auxo->nodeValueQuery(a,0,nodeinflowtime);
            clock_gettime(CLOCK_MONOTONIC, &te);
            //sumtime2 = timespec_diff(&ts, &te);
            ofin<<sumtime2<<endl;

            auxo->nodeValueQuery(b,0,nodeinflowtime);
            auxopro->nodeValueQuery(a,0,nodeinflowprotime);
            auxopro->nodeValueQuery(b,0,nodeinflowprotime);
            //
            clock_gettime(CLOCK_MONOTONIC, &ts);
            auxo->nodeValueQuery(a,1,nodeoutflowtime);
            clock_gettime(CLOCK_MONOTONIC, &te);
            //sumtime3 = timespec_diff(&ts, &te);
            ofout<<sumtime3<<endl;
            auxo->nodeValueQuery(b,1,nodeoutflowtime);
            auxopro->nodeValueQuery(a,1,nodeoutflowprotime);
            auxopro->nodeValueQuery(b,1,nodeoutflowprotime);
            //
            clock_gettime(CLOCK_MONOTONIC, &ts);
            auxo->reachQuery(a,b,reachabilitytime);
            clock_gettime(CLOCK_MONOTONIC, &te);
            //sumtime4 = timespec_diff(&ts, &te);
            ofreach<<sumtime4<<endl;

            auxopro->reachQuery(a,b,reachabilityprotime);

        }

        



        out1 <<"****************** AUXO edge Weight Query start *****************" << endl;
        out1<<"had query "<<count<<"data"<<endl;
        out1<<"query time "<<edgequerytime<<"usecond"<<endl;
        out1<<"averge query time "<<(edgequerytime/count)<<"usecond"<<endl;
        out1 <<"****************** AUXO edge Weight Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        out1 <<"****************** AUXOpro edge Weight Query start *****************" << endl;
        out1<<"had query "<<count<<"data"<<endl;
        out1<<"query time "<<edgequeryprotime<<"usecond"<<endl;
        out1<<"averge query time "<<(edgequeryprotime/count)<<"usecond"<<endl;
        out1 <<"****************** AUXOpro edge Weight Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        //
        out1 <<"****************** AUXO Node in-flow Query start *****************" << endl;
        out1<<"had query "<<2*count<<"data"<<endl;
        out1<<"query time "<<nodeinflowtime<<"usecond"<<endl;
        out1<<"averge query time "<<(nodeinflowtime/(2*count))<<"usecond"<<endl;
        out1 <<"****************** AUXO Node in-flow Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        out1 <<"****************** AUXOpro Node in-flow Query start *****************" << endl;
        out1<<"had query "<<2*count<<"data"<<endl;
        out1<<"query time "<<nodeinflowprotime<<"usecond"<<endl;
        out1<<"averge query time "<<(nodeinflowprotime/(2*count))<<"usecond"<<endl;
        out1 <<"****************** AUXOpro Node in-flow Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        //
        out1 <<"****************** AUXO Node out-flow Query start *****************" << endl;
        out1<<"had query "<<2*count<<"data"<<endl;
        out1<<"query time "<<nodeoutflowtime<<"usecond"<<endl;
        out1<<"averge query time "<<(nodeoutflowtime/(2*count))<<"usecond"<<endl;
        out1 <<"****************** AUXO Node out-flow Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        out1 <<"****************** AUXOpro Node out-flow Query start *****************" << endl;
        out1<<"had query "<<2*count<<"data"<<endl;
        out1<<"query time "<<nodeoutflowprotime<<"usecond"<<endl;
        out1<<"averge query time "<<(nodeoutflowprotime/(2*count))<<"usecond"<<endl;
        out1 <<"****************** AUXOpro Node out-flow Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        //
        out1 <<"****************** AUXO Reachability Query start *****************" << endl;
        out1<<"had query "<<count<<"data"<<endl;
        out1<<"query time "<<reachabilitytime<<"usecond"<<endl;
        out1<<"averge query time "<<(reachabilitytime/count)<<"usecond"<<endl;
        out1 <<"****************** AUXO Reachabilit Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        out1 <<"****************** AUXOpro Reachabilit Query start *****************" << endl;
        out1<<"had query "<<count<<"data"<<endl;
        out1<<"query time "<<reachabilityprotime<<"usecond"<<endl;
        out1<<"averge query time "<<(reachabilityprotime/count)<<"usecond"<<endl;
        out1 <<"****************** AUXOpro Reachabilit Query end *****************" << endl;
        out1<<endl<<endl<<endl;
        //
        


    }*/
   
    ofstream of("..//result//delitopkcdf.txt");
    ifstream ifs1;
    string queryfilepath="..\\..\\..\\TestFile\\Delicious-ui\\delicious-uiqueryfilecdf";
    ifs1.open(queryfilepath);
	if (!ifs1.is_open()) {
        cout << "Error in open file, Path = " << queryfilepath << endl;
		return -1;
        
    }
    vector<string> query={"459492","22908","103483","3976","1550284","925505","62801","732265"};
    timespec ts, te;

    while(!ifs1.eof()){
        string a,b;
        ifs1>>a>>b;
        query.insert(query.end(),a);
        query.insert(query.end(),b);
        clock_gettime(CLOCK_MONOTONIC, &ts);
        auxo->TopKheavyhitter(query);
        
        clock_gettime(CLOCK_MONOTONIC, &te);
        query.pop_back();
        query.pop_back();
        long long sumtime = timespec_diff(&ts, &te);
        of<<sumtime<<endl;
    }

    return 0;
}
