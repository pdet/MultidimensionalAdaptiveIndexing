#ifndef INDEX_FACTORY_H
#define INDEX_FACTORY_H

#include "abstract_index.hpp"
#include "full_scan.hpp"
#include "full_scan_candidate_list.hpp"
#include "full_scan_slow.hpp"
#include "median_kd_tree.hpp"
#include "average_kd_tree.hpp"
#include "quasii.hpp"
#include "cracking_kd_tree.hpp"
#include "cracking_kd_tree_per_dimension.hpp"
#include "cracking_kd_tree_pre_processing.hpp"
#include "progressive_index.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

using namespace std;

class IndexFactory
{
public:
    static unique_ptr<AbstractIndex> getIndex(
            size_t algorithm_id,
            map<string, string> config = map<string, string>()
        ){
        switch(algorithm_id){
            case FullScan::ID:
                return make_unique<FullScan>(config);
            case FullScanCandidateList::ID:
                return make_unique<FullScanCandidateList>(config);
            case FullScanSlow::ID:
                return make_unique<FullScanSlow>(config);
            case MedianKDTree::ID:
                return make_unique<MedianKDTree>(config);
            case CrackingKDTree::ID:
                return make_unique<CrackingKDTree>(config);
            case CrackingKDTreePreProcessing::ID:
                return make_unique<CrackingKDTreePreProcessing>(config);
            case CrackingKDTreePerDimension::ID:
                return make_unique<CrackingKDTreePerDimension>(config);
            case AverageKDTree::ID:
                return make_unique<AverageKDTree>(config);
            case Quasii::ID:
                return make_unique<Quasii>(config);
            case ProgressiveIndex::ID:
                return make_unique<ProgressiveIndex>(config);
            default:
                throw std::invalid_argument("Invalid Algorithm ID");
                assert(false);
        }
    }

    static vector<size_t> algorithmIDs(){
        return {
                FullScan::ID,
                MedianKDTree::ID,
                CrackingKDTree::ID,
                CrackingKDTreePerDimension::ID,
                AverageKDTree::ID,
                Quasii::ID
        };
    }

    static shared_ptr<AbstractIndex> baselineIndex(
            map<string, string> config = map<string, string>()
        ){
        return make_unique<FullScanSlow>(config);
    }
};
#endif // INDEX_FACTORY_H
