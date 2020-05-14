#pragma once
#include "abstract_index.hpp"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <kd_tree/kd_tree.hpp>
#include <memory>
#include <vector>

class ProgressiveIndex: public AbstractIndex {

  public:
    static const size_t ID = 7;
  string name() override{
    return "ProgressiveIndexing";
  }
  ProgressiveIndex(std::map<std::string, std::string> config);
  ~ProgressiveIndex();
  void initialize(Table *table_to_copy) override;

  void adapt_index(Query &query) override;

  unique_ptr<Table> range_query(Query &query) override;

  unique_ptr<Table> progressive_quicksort(Query &query);

  //! Progressive KD-Tree Index Root
    unique_ptr<KDTree> tree;
    size_t current_position = 0;
    unique_ptr<vector<KDNode*>> refinement_nodes;
    size_t node_being_refined = 0;
    double delta = 0.2;
    //! In the workload adaptive version we prioritize pieces that will be accessed by the query
    bool workload_adaptive = false;
    //! If our index is fully converged
    bool converged = false;
    //! Adaptive Delta
    double full_scan_time = 0;
    double interactivity_threshold = 0;
    ProgressiveIndex() :  tree(nullptr), current_position(0){};

    void initializeRoot(float pivot, size_t tableSize) {
        assert(!tree);
        tree = std::make_unique<KDTree>(tableSize);
        tree->root = make_unique<KDNode>(0,pivot,0,tableSize-1);
    }
    double get_costmodel_delta_quicksort(std::vector<int64_t>& originalColumn, int64_t low, int64_t high, double delta);

private:
    Table *originalTable = nullptr;
    size_t minimum_partition_size = 1024;
    unique_ptr<Table> progressive_quicksort_create(Query &query, ssize_t &remaining_swaps);
    void progressive_quicksort_refine(Query& query, ssize_t& remaining_swaps);
    void workload_dependent_refine(Query &query, ssize_t &remaining_swaps);
    void workload_agnostic_refine(Query &query, ssize_t &remaining_swaps);
};
