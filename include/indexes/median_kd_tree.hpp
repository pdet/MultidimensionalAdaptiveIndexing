#ifndef MEDIAN_KD_TREE_H
#define MEDIAN_KD_TREE_H

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"

class MedianKDTree : public AbstractIndex
{
public:
    MedianKDTree();
    ~MedianKDTree();

    string name(){
        return "Median KD-Tree";
    }

    void initialize(const shared_ptr<Table> table_to_copy);

    void adapt_index(Query& query);

    shared_ptr<Table> range_query(Query& query);

private:
    unique_ptr<KDTree> index;
    const size_t minimum_partition_size = 100;

    // Variables to help with initialization
    vector<size_t> columns;
    vector<size_t> lower_limits, upper_limits;
    vector<KDNode> nodes_to_check;

    unique_ptr<KDTree> initialize_index();

    pair<float, size_t> find_median(size_t column, size_t lower_limit, size_t upper_limit);

    // Returns the position on where the pivot would end
    size_t pivot_table(size_t column, size_t low, size_t high, float pivot, size_t pivot_position);
};
#endif // MEDIAN_KD_TREE_H