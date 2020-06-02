#ifndef CRACKING_KD_TREE_HPP
#define CRACKING_KD_TREE_HPP

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>
#include <set>

class CrackingKDTree : public AbstractIndex
{
    public:
    bool should_adapt = true;
    static const size_t ID = 2;
    CrackingKDTree(std::map<std::string, std::string> config);
    ~CrackingKDTree();

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

    void adapt(Query& query);
    void adapt_recursion(
        KDNode *current, Query& query,
        size_t pivot_dim, float pivot, size_t lower_limit, size_t upper_limit
    );
};

#endif
