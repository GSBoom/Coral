#ifndef HOT_H
#define HOT_H

#include <iostream>
#include <vector>
#include <random>
#include <ctime>    // time() functions
#include <chrono>
#include <cmath>
#include <functional>
#include <map>
#include "BloomFilter.h"

using namespace std;



int generateRandomNumber() {
    // Create a random number engine. Here, the default random number engine is used.
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(timestamp);

    uniform_int_distribution<> dis(0, 1000);

    int count=0;
    while(1){
        //cout<<dis(gen)<<" ";
        if(dis(gen)%2==0)return count;
        count++;
    }

    return 0;

}


int generateRandomNumber(int k) {
    // Create a random number engine. Here, the default random number engine is used.
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(timestamp);
    uniform_int_distribution<> dis(0, k);
    return dis(gen);
   

}


class HotIdentify{


    public:
    map<int,int> potentialhotrow;//potential hot row
    int CompareNumber;//The criteria for becoming a potential hot row: The coin lands heads up more than 2^n times.
    int HotrowTimes;//The criteria for becoming a hot row : n times be potential hot row
    BloomFilter bloomfilter;//store hot rows information
    HotIdentify(int maxhotrownumber,double precisionrate,int comparenumber,int hotrowtimes):bloomfilter(maxhotrownumber,precisionrate){
        CompareNumber=comparenumber;
        HotrowTimes=hotrowtimes;
    }
    void update(int rownumber);




};

void HotIdentify::update(int rownumber){
    int answer=generateRandomNumber();
    if(answer>CompareNumber){
        if (potentialhotrow.find(rownumber) != potentialhotrow.end()) {
            // has existed add 1
            potentialhotrow[rownumber]++;
        } else {
            // If it doesn't exist, insert the key and initialize the value to 1.
            potentialhotrow[rownumber] = 1;
        }
        if(potentialhotrow[rownumber]>HotrowTimes){
            bloomfilter.insert(to_string(rownumber));
        }
    }
    return;

}


#endif // HOT_H