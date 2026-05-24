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
    //cout<<"generateRandomNumber()"<<endl;

    // 创建一个随机数引擎，这里使用默认的随机数引擎
    
    //auto timestamp = std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count();
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(timestamp);

    // 创建一个均匀分布，生成 0 到 1000 之间的随机整数
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
    //cout<<"generateRandomNumber(k)"<<endl;

    // 创建一个随机数引擎，这里使用默认的随机数引擎
    
    //auto timestamp = std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count();
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(timestamp);

    // 创建一个均匀分布，生成 0 到 k 之间的随机整数
    uniform_int_distribution<> dis(0, k);
    return dis(gen);
   

}


class HotIdentify{


    public:
    map<int,int> potentialhotrow;//潜在热行
    int CompareNumber;//成为预备热行的标准 >n次朝上
    int HotrowTimes;//成为热行的标准 有多少次大于n次朝上
    BloomFilter bloomfilter;//热行的存储位置
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
            // 键存在，值加 1
            potentialhotrow[rownumber]++;
        } else {
            // 键不存在，插入键并将值初始化为 1
            potentialhotrow[rownumber] = 1;
        }
        if(potentialhotrow[rownumber]>HotrowTimes){
            bloomfilter.insert(to_string(rownumber));
        }
    }
    return;

}


#endif // HOT_H