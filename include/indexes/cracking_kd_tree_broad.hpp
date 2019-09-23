#ifndef CRACKING_KDTREE_BROAD_H
#define CRACKING_KDTREE_BROAD_H

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"

class CrackingKDTreeBroad : public AbstractIndex
{
public:
    CrackingKDTreeBroad();
    ~CrackingKDTreeBroad();

    string name() override{
        return "Cracking KD-Tree Broad";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;
private:
    unique_ptr<KDTree> index;
    const size_t minimum_partition_size = 100;

    // Vectors to simplify the insertion algorithm
    vector<KDNode*> nodes_to_check;
    vector<size_t> lower_limits, upper_limits;

    void insert(size_t column, float key);

    void follow_or_crack_right(KDNode *current, size_t column, float key, float upper_limit);

    void follow_or_crack_left(KDNode *current, size_t column, float key, float lower_limit);
};
#endif // CRACKING_KDTREE_BROAD_H
