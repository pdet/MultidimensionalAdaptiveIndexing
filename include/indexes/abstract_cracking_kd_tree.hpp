#ifndef ABSTRACT_KDTREE_H
#define ABSTRACT_KDTREE_H

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <map>
#include <string>

class AbstractCrackingKDTree : public AbstractIndex
{
public:
    AbstractCrackingKDTree(std::map<std::string, std::string> config);
    ~AbstractCrackingKDTree();

    string name() override{
        return "Abstract Craking KD-Tree";
    }

    void initialize(Table *table_to_copy) override;

    // Doesn't implement the adapt_index method
    // void adapt_index(Query& query);

    Table range_query(Query& query) override;

    void draw_index(std::string path) override;
protected:
    unique_ptr<KDTree> index;
    int64_t minimum_partition_size = 100;
    std::vector<int64_t> lower_limits;
    std::vector<int64_t> upper_limits;
    std::vector<KDNode*> nodes_to_check;

    void insert(Query& query, int64_t column, float key);

    void follow_left_or_crack(KDNode *current, Query& limits, int64_t column, float key, int64_t lower_limit);

    void follow_right_or_crack(KDNode *current, Query& limits, int64_t column, float key, int64_t upper_limit);
};
#endif // ABSTRACT_KDTREE_H
