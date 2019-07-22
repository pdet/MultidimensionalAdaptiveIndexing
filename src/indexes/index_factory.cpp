#ifndef INDEX_FACTORY
#define INDEX_FACTORY

#include "indexes.cpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class IndexFactory
{
public:
    static unique_ptr<AbstractIndex> getIndex(string index_name){
        if(index_name.compare("Full-Scan"))
            return make_unique<FullScan>();
        if(index_name.compare("Cracking-KDTree-Broad"))
            return make_unique<CrackingKDTreeBroad>();
        // if(index_name.compare("Cracking-KDTree-Narrow"))
        //     return make_unique<CrackingKDTreeNarrow>();
        return make_unique<FullScan>();
    }

    static vector<unique_ptr<AbstractIndex>> allIndexes(){
        vector<unique_ptr<AbstractIndex>> result;
        result.push_back(make_unique<CrackingKDTreeBroad>());
        // result.push_back(make_unique<CrackingKDTreeNarrow>());
        return result;
    }

    static unique_ptr<AbstractIndex> baseline_index(){
        return make_unique<FullScan>();
    }
};
#endif