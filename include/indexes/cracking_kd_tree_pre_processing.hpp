#pragma once
#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>
#include <set>
#include <cost_model.hpp>

class CrackingKDTreePreProcessing : public AbstractIndex
{
    public:
    bool should_adapt = true;
    static const size_t ID = 21;
    CrackingKDTreePreProcessing(std::map<std::string, std::string> config);
    ~CrackingKDTreePreProcessing();

    std::string name() override{
        return "CrackingKDTree";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query &query) override;

  unique_ptr<Table> range_query(Query &query) override;

    void draw_index(std::string path) override{
        index->draw(path);
    }

    bool sanity_check() override{
        return index->sanity_check(table.get());
    }

private:
    std::unique_ptr<KDTree> index;
    size_t minimum_partition_size = 100;
    double interactive_threshold = 0.5;


    CostModel cost_model;
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

    void adapt(Query& query);
    void adapt_recursion(
        KDNode *current, Query& query,
        size_t pivot_dim, float pivot, size_t lower_limit, size_t upper_limit
    );

    std::pair<float, size_t> find_average(size_t column, size_t lower_limit, size_t upper_limit);
    void initialize_index_recursion(
            KDNode* current, size_t lower_limit, size_t upper_limit, size_t column, size_t partition_size
    );
    void initialize_index(size_t partition_size);
};
