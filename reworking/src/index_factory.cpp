#ifndef INDEX_FACTORY
#define INDEX_FACTORY

#include "indexes/indexes.cpp"
#include <iostream>
#include <string>

using namespace std;

class IndexFactory
{
public:
    static unique_ptr<AbstractIndex> getIndex(string index_name){
        if(index_name.compare("Full-Scan"))
            return make_unique<FullScan>();
        return make_unique<FullScan>();
    }
};
#endif