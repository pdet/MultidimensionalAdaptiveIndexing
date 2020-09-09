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

  bool sanity_check() override{
      return tree->sanity_check(table.get());
  }

  //! Progressive KD-Tree Index Root
    unique_ptr<KDTree> tree;
    size_t current_position = 0;
    unique_ptr<vector<KDNode*>> refinement_nodes;
    size_t node_being_refined = 0;
    //! tells us if we are using a fixed_delta
    bool is_delta_fixed = false;
    double delta = 0.2;
    double sum_delta = 0.0;
    //! In the workload adaptive version we prioritize pieces that will be accessed by the query
    //! If our index is fully converged
    bool converged = false;
    //! This keeps track if the cost is over the interactive threshold
    bool interactive_threshold_is_over = false;
    bool interactive_threshold_is_time = false;
    //! Adaptive Delta
    double interactivity_threshold = 0;
    //! Interactivity threshold used over regular interactivity threhsold
    double interactivity_threshold_over = 0;
    double cur_interactivity_threshold = 0;
    //! Cost to write one page sequentially
    double WRITE_ONE_PAGE_SEQ_MS = 0;
    //! num queries that will be over interactivity threshold
    int num_queries_over = 0;
    //! Cost to read one page without checks
    double READ_ONE_PAGE_WITHOUT_CHECKS_SEQ_MS = 0;
    //! Cost to read one page sequentially with checks
    double READ_ONE_PAGE_SEQ_MS = 0;
    //! Cost of randomly accessing one page
    double RANDOM_ACCESS_PAGE_MS = 0;
    //! Cost of swapping one page
    double SWAP_COST_PAGE_MS = 0;
    double first_col_cr = 0;
    double extra_col_cr = 0;
    //! How many elements per page
    size_t PAGESIZE = 4096;
    size_t ELEMENTS_PER_PAGE = PAGESIZE/sizeof(float);
    ssize_t fq_remaining_swaps = 0;
    bool log_q = false;
    ProgressiveIndex() :  tree(nullptr), current_position(0){};

    void initializeRoot(float pivot, size_t tableSize) {
        assert(!tree);
        tree = std::make_unique<KDTree>(tableSize);
        tree->root = make_unique<KDNode>(0,pivot,0,tableSize-1);
    }
    double get_delta_react();
    double get_delta(Query &query);

private:
    Table *originalTable = nullptr;
    size_t minimum_partition_size = 1024;
    unique_ptr<Table> progressive_quicksort_create(Query &query, ssize_t &remaining_swaps);
    void progressive_quicksort_refine(Query& query, ssize_t& remaining_swaps);
    void workload_dependent_refine(Query &query, ssize_t &remaining_swaps);
    void workload_agnostic_refine(Query &query, ssize_t &remaining_swaps);
};
