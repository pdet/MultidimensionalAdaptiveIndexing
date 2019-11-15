#ifndef MEDIAN_KD_TREE_H
#define MEDIAN_KD_TREE_H

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <map>
#include <string>

class MedianKDTree : public AbstractIndex
{
public:
    MedianKDTree(std::map<std::string, std::string> config);
    ~MedianKDTree();

    string name() override{
        return "Median KD-Tree";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    // Todo: this is public because of the medians generator, but it shouldn't
    unique_ptr<KDTree> index;

    void draw_index(std::string path) override{
        index->draw(path);
    }

private:
    int64_t minimum_partition_size = 100;

    // Variables to help with initialization
    vector<int64_t> columns;
    vector<int64_t> lower_limits, upper_limits;
    vector<KDNode*> nodes_to_check;

    unique_ptr<KDTree> initialize_index();

    pair<float, int64_t> find_median(int64_t column, int64_t lower_limit, int64_t upper_limit);

    // Returns the position on where the pivot would end
    int64_t pivot_table(int64_t column, int64_t low, int64_t high, float pivot, int64_t pivot_position);
};
#endif // MEDIAN_KD_TREE_H
