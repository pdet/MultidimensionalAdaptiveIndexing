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
    static const size_t ID = 5;
    MedianKDTree(std::map<std::string, std::string> config);
    ~MedianKDTree();

    std::string name() override{
        return "MedianKDTree";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    std::unique_ptr<Table> range_query(Query& query) override;

    void draw_index(std::string path) override{
        index->draw(path);
    }

    bool sanity_check() override{
        return index->sanity_check(table.get());
    }

private:
    std::unique_ptr<KDTree> index;
    size_t minimum_partition_size = 100;

    void initialize_index();
    void initialize_index_recursion(KDNode* current, size_t lower_limit, size_t upper_limit, size_t column);

    std::pair<float, size_t> find_median(size_t column, size_t lower_limit, size_t upper_limit);

    // Returns the position on where the pivot would end
    size_t pivot_table(size_t column, size_t low, size_t high, float pivot, size_t pivot_position);
};
#endif // MEDIAN_KD_TREE_H
