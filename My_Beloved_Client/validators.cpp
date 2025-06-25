#include "validators.h"

validators::validators() {}

bool validators::IsAddressValid(QString Address)
{
    int DotCount = 0;
    for(int i = 0;i < Address.size();++i) {
        if(Address[i] == '.') {
            ++DotCount;
        }
    }
    if(DotCount != 3) {
        return false;
    }
    if(DotCount == 3 && Address.size() == 3) {
        return false;
    }
    return true;

}
bool validators::IsPortValid(QString Port)
{
    int DotCount = 0;
    for(int i = 0;i < Port.size();++i) {
        if(Port[i] == '.') {
            ++DotCount;
        }
    }
    if(DotCount != 3) {
        return false;
    }
    if(DotCount == 3 && Port.size() < 7 ) {
        return false;
    }
    return true;

}
