#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

using namespace std;

class BloomFilter {
private:
    vector<bool> bitArray;  // bit array
    int numBits;                 // size of bit arrary
    int numHashFunctions;        // number of hash function

    // hash functions
    hash<string> hashFunction;

    // generate series of hash value
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

    // Initialize
    BloomFilter(int expectedElements, double falsePositiveRate) {
        // Calculate the size of the bit array based on the expected number of elements and the false positive rate
        numBits = - (expectedElements * log(falsePositiveRate)) / (log(2) * log(2));
        
        numHashFunctions = (numBits / expectedElements) * log(2);

        
        bitArray.resize(numBits, false);
    }

    // insert
    void insert(const string& key) {
        vector<int> hashValues = getHashValues(key);
        for (int hashValue : hashValues) {
            bitArray[hashValue] = true;
        }
    }

    // existence check
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