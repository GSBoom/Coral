#ifndef BASKETUSED_H
#define BASKETUSED_H

#include <cstdint>

using namespace std;

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
        // 先比较 globaluse，如果 globaluse 相等再比较 localuse
        if (globaluse != other.globaluse) {
            return globaluse < other.globaluse;
        }
        return localuse < other.localuse;
    }


    ~basketused() {}


private:
    
};



#endif//BASKETUSED_H

