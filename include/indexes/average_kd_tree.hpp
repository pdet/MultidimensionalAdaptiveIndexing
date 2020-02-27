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
    static const size_t ID = 3;
    AverageKDTree(std::map<std::string, std::string> config);
    ~AverageKDTree();

    string name() override{
        return "Average KD-Tree";
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

    // Variables to help with initialization
    vector<int64_t> columns;
    vector<int64_t> lower_limits, upper_limits;
    vector<KDNode*> nodes_to_check;

    unique_ptr<KDTree> initialize_index();

    pair<float, int64_t> find_average(int64_t column, int64_t lower_limit, int64_t upper_limit);
};
#endif // AVERAGE_KD_TREE_H
