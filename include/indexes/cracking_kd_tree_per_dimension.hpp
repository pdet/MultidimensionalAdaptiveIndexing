#ifndef CRACKING_KD_TREE_PER_DIMENSION_HPP
#define CRACKING_KD_TREE_PER_DIMENSION_HPP

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>
#include <set>

class CrackingKDTreePerDimension : public AbstractIndex
{
    public:
    static const size_t ID = 90;
    CrackingKDTreePerDimension(std::map<std::string, std::string> config);
    ~CrackingKDTreePerDimension();

    string name() override{
        return "CrackingKDTreePerDimension";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    void draw_index(std::string path) override{
        index->draw(path);
    }

private:
    unique_ptr<KDTree> index;
    int64_t minimum_partition_size = 100;

    void adapt(Query& query);
    void adapt_recursion(
        KDNode *current, Query& query,
        int64_t pivot_dim, float pivot, int64_t lower_limit, int64_t upper_limit
    );
};

#endif
