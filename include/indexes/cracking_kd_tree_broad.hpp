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

    string name(){
        return "Cracking KD-Tree Broad";
    }

    void initialize(const shared_ptr<Table> table_to_copy);

    void adapt_index(Query& query);

    shared_ptr<Table> range_query(Query& query);
private:
    unique_ptr<KDTree> index;
    const size_t minimum_partition_size = 100;

    // Vectors to simplify the insertion algorithm
    vector<shared_ptr<KDNode>> nodes_to_check;
    vector<size_t> lower_limits, upper_limits;

    void insert(size_t column, float key);

    void follow_or_crack_right(shared_ptr<KDNode> current, size_t column, float key, float upper_limit);

    void follow_or_crack_left(shared_ptr<KDNode> current, size_t column, float key, float lower_limit);
};
#endif // CRACKING_KDTREE_BROAD_H