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
    int64_t sum = 0;

    void reserve(size_t capacity) { (void)capacity; }

    inline void push_back(int64_t value) { sum += value; }
    inline void push_back(IdxColEntry value) { sum += value.m_key; }

    inline void maybe_push_back(IdxColEntry value, int maybe) { sum += maybe * value.m_key; }
    inline void maybe_push_back(int64_t value, int maybe) { sum += maybe * value; }
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
  string name() override{
    return "ProgressiveIndexing";
  }

  void initialize(Table *table_to_copy) override;

  void adapt_index(Table *originalTable,Query& query) override;

  Table range_query(Table *originalTable,Query& query) override;

  void progressive_quicksort(Table *originalTable,Query& query);

  //! Progressive Avl Index Root
    unique_ptr<KDTree> tree;
    size_t current_position = 0;

    //! FIXME : For now fixing delta
    double delta = 0.1;

    //! Adaptive Delta
    double full_scan_time = 0;
    double interactivity_threshold = 0;
    ProgressiveIndex() :  tree(nullptr), current_position(0){};

    void initializeRoot(int64_t pivot, size_t tableSize) {
        assert(!tree);
        tree = std::make_unique<KDTree>(tableSize);
    }
    double get_costmodel_delta_quicksort(std::vector<int64_t>& originalColumn, int64_t low, int64_t high, double delta);
};
