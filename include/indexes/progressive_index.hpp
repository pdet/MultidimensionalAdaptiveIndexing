#pragma once
#include "abstract_index.hpp"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <kd_tree/kd_tree.hpp>
#include <memory>
#include <vector>

class ResultStruct {
  public:
    ResultStruct() : sum(0){};
    float sum = 0;

    void reserve(size_t capacity) { (void)capacity; }

    inline void push_back(double value) { sum += value; }

    inline void maybe_push_back(double value, int maybe) { sum += maybe * value; }
    inline void merge(ResultStruct other) { sum += other.sum; }
};

class RefinementScan{
  public:
    RefinementScan():offsetLeft(-1),offsetLeftMiddle(-1),offsetRightMiddle(-1),offsetRight(-1),checkLeft(true),checkRight(true){};
    int64_t offsetLeft;
    int64_t offsetLeftMiddle;
    int64_t offsetRightMiddle;
    int64_t offsetRight;
    bool  checkLeft;
    bool  checkRight;
};

class ProgressiveIndex: public AbstractIndex {

  public:
    static const size_t ID = 7;
  string name() override{
    return "ProgressiveIndexing";
  }
  ProgressiveIndex(std::map<std::string, std::string> config);
  ~ProgressiveIndex();
  void initialize(Table *table_to_copy) override;

  void adapt_index(Table *originalTable,Query& query) override;

  std::unique_ptr<Table> range_query(Table *originalTable,Query& query) override;

  unique_ptr<Table> progressive_quicksort(Table *originalTable,Query& query);

  //! Progressive KD-Tree Index Root
    unique_ptr<KDTree> tree;
    size_t current_position = 0;

    //! FIXME : For now fixing delta
    double delta = 0.1;

    //! Adaptive Delta
    double full_scan_time = 0;
    double interactivity_threshold = 0;
    ProgressiveIndex() :  tree(nullptr), current_position(0){};

    void initializeRoot(float pivot, size_t tableSize) {
        assert(!tree);
        tree = std::make_unique<KDTree>(tableSize);
        tree->root = make_unique<KDNode>(0,pivot,0,tableSize);
    }
    double get_costmodel_delta_quicksort(std::vector<int64_t>& originalColumn, int64_t low, int64_t high, double delta);

private:
    size_t minimum_partition_size = 100;
    unique_ptr<Table> progressive_quicksort_create(Table *originalTable,Query& query, ssize_t& remaining_swaps);
};
