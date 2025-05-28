#ifndef BASKETUSED_H
#define BASKETUSED_H

#include <cstdint>

using namespace std;
//this is a struct addition to coral ,in order to visualize the situation inside the buckets of Coral.
class basketused
{
public:
    uint16_t localuse;
    uint16_t globaluse;
    basketused(){
        localuse=0;
        globaluse=0;
    }
    basketused(uint16_t a,uint16_t b) {
        localuse=a;
        globaluse=b;

    }

    bool operator<(const basketused& other) const {
        
        if (globaluse != other.globaluse) {
            return globaluse < other.globaluse;
        }
        return localuse < other.localuse;
    }


    ~basketused() {}


private:
    
};



#endif//BASKETUSED_H

