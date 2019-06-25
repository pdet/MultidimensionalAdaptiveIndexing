#ifndef INDEX_FACTORY
#define INDEX_FACTORY

#include "indexes/indexes.cpp"
#include <iostream>
#include <string>

using namespace std;

class IndexFactory
{
public:
    static AbstractIndex* getIndex(string index_name){
        if(index_name.compare("Full-Scan"))
            return new FullScan();
        return new FullScan();
    }
};
#endif