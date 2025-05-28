#include "querysupportstruct.h"
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <set>

using namespace std;
int main1(int argc, char* argv[])
{
    string dataset="../../data/lkml/rawdata.txt";
    string nodeOut="../../data/lkml/nodeOutQueryTest.txt";
    string nodeIn="../../data/lkml/nodeInQueryTest.txt";
    string edge="../../data/lkml/edgeQueryTest.txt";
    string result="../result/usa-query.txt";
    string queryfile="..\\..\\..\\TestFile\\USA-Road\\usa-roadqueryfile";
    vector<int> v={20,40,60,80,100};

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
        ifs >> s >> d>> w >> t;
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
/*
    for(int num:v){
        string queryfilepath=queryfile+to_string(num);
        ifstream ifs1;
        double edgequerytime=0;
        double nodeinflowtime=0;
        double nodeoutflowtime=0;
        double reachabilitytime=0;

        double edgequeryprotime=0;
        double nodeinflowprotime=0;
        double nodeoutflowprotime=0;
        double reachabilityprotime=0;

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
            auxo->edgeQuery(a,b,edgequerytime);
            auxopro->edgeQuery(a,b,edgequeryprotime);
            //
            auxo->nodeValueQuery(a,0,nodeinflowtime);
            auxo->nodeValueQuery(b,0,nodeinflowtime);
            auxopro->nodeValueQuery(a,0,nodeinflowprotime);
            auxopro->nodeValueQuery(b,0,nodeinflowprotime);
            //
            auxo->nodeValueQuery(a,1,nodeoutflowtime);
            auxo->nodeValueQuery(b,1,nodeoutflowtime);
            auxopro->nodeValueQuery(a,1,nodeoutflowprotime);
            auxopro->nodeValueQuery(b,1,nodeoutflowprotime);
            //
            auxo->reachQuery(a,b,reachabilitytime);
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
        


    }
*/

    return 0;
}
