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
        if(index_name.compare("KDTree-Median"))
            return make_unique<MedianKDTree>();
        if(index_name.compare("KDTree-Average"))
            return make_unique<AverageKDTree>();
        return make_unique<FullScan>();
    }

    static vector<shared_ptr<AbstractIndex>> allIndexes(){
        vector<shared_ptr<AbstractIndex>> indexes;
        // indexes.push_back(make_unique<CrackingKDTreeBroad>());
        // indexes.push_back(make_unique<CrackingKDTreeNarrow>());
        // indexes.push_back(make_unique<MedianKDTree>());
        indexes.push_back(make_unique<AverageKDTree>());
        return indexes;
    }

    static shared_ptr<AbstractIndex> baseline_index(){
        return make_unique<FullScan>();
    }
};
#endif