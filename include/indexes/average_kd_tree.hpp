#ifndef AVERAGE_KD_TREE_H
#define AVERAGE_KD_TREE_H

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>

class AverageKDTree : public AbstractIndex
{
public:
    AverageKDTree(std::map<std::string, std::string> config);
    ~AverageKDTree();

    string name() override{
        return "Average KD-Tree";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

private:
    unique_ptr<KDTree> index;
    int64_t minimum_partition_size = 100;

    // Variables to help with initialization
    vector<size_t> columns;
    vector<size_t> lower_limits, upper_limits;
    vector<KDNode*> nodes_to_check;

    unique_ptr<KDTree> initialize_index();

    pair<float, size_t> find_average(size_t column, size_t lower_limit, size_t upper_limit);
};
#endif // AVERAGE_KD_TREE_H
