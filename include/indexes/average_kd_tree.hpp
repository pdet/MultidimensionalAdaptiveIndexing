#ifndef AVERAGE_KD_TREE_H
#define AVERAGE_KD_TREE_H

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"

class AverageKDTree : public AbstractIndex
{
public:
    AverageKDTree();
    ~AverageKDTree();

    string name(){
        return "Average KD-Tree";
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

    pair<float, size_t> find_average(size_t column, size_t lower_limit, size_t upper_limit);
};
#endif // AVERAGE_KD_TREE_H