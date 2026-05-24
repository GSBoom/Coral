#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

using namespace std;

class BloomFilter {
private:
    vector<bool> bitArray;  // 位数组
    int numBits;                 // 位数组的大小
    int numHashFunctions;        // 哈希函数的数量

    // 哈希函数
    hash<string> hashFunction;

    // 生成多个哈希值
    vector<int> getHashValues(const string& key) {
        vector<int> hashValues;
        for (int i = 0; i < numHashFunctions; ++i) {
            size_t hashValue = hashFunction(key + to_string(i));
            hashValues.push_back(hashValue % numBits);
        }
        return hashValues;
    }
    
    public:
    BloomFilter(){
        numBits=256;
        numHashFunctions=8;
        bitArray.resize(numBits, false);


    }

    // 构造函数，初始化布隆过滤器
    BloomFilter(int expectedElements, double falsePositiveRate) {
        // 根据预期元素数量和误判率计算位数组的大小
        numBits = - (expectedElements * log(falsePositiveRate)) / (log(2) * log(2));
        // 计算哈希函数的数量
        numHashFunctions = (numBits / expectedElements) * log(2);

        // 初始化位数组
        bitArray.resize(numBits, false);
    }

    // 插入元素
    void insert(const string& key) {
        vector<int> hashValues = getHashValues(key);
        for (int hashValue : hashValues) {
            bitArray[hashValue] = true;
        }
    }

    // 检查元素是否存在
    bool mayContain(const string& key) {
        vector<int> hashValues = getHashValues(key);
        for (int hashValue : hashValues) {
            if (!bitArray[hashValue]) {
                return false;
            }
        }
        return true;
    }
};
#endif //BLOOM_FILTER_H