#ifndef INDEX_FACTORY_H
#define INDEX_FACTORY_H

#include "abstract_index.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree_broad.hpp"
#include "cracking_kd_tree_narrow.hpp"
#include "median_kd_tree.hpp"
#include "average_kd_tree.hpp"
#include "quasii.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class IndexFactory
{
public:
    static unique_ptr<AbstractIndex> getIndex(string index_name){
        if(index_name == "Full-Scan")
            return make_unique<FullScan>();
        if(index_name == "Cracking KD-Tree Broad")
            return make_unique<CrackingKDTreeBroad>();
        // if(index_name == "Cracking KD-Tree Narrow")
        //     return make_unique<CrackingKDTreeNarrow>();
        if(index_name == "KDTree-Median")
            return make_unique<MedianKDTree>();
        if(index_name == "KDTree-Average")
            return make_unique<AverageKDTree>();
        if(index_name == "Quasii")
            return make_unique<Quasii>();
        assert(false);
    }

    static vector<shared_ptr<AbstractIndex>> allIndexes(){
        vector<shared_ptr<AbstractIndex>> indexes;
        indexes.push_back(make_unique<CrackingKDTreeBroad>());
        // indexes.push_back(make_unique<CrackingKDTreeNarrow>());
        indexes.push_back(make_unique<MedianKDTree>());
        indexes.push_back(make_unique<AverageKDTree>());
        // indexes.push_back(make_unique<Quasii>());
        return indexes;
    }

    static shared_ptr<AbstractIndex> baselineIndex(){
        return make_unique<FullScan>();
    }
};
#endif // INDEX_FACTORY_H
