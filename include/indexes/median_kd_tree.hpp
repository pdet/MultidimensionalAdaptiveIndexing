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

    void adapt_index(Table *originalTable,Query& query) override;

  std::unique_ptr<Table>  range_query(Table *originalTable,Query& query) override;



    void draw_index(std::string path) override{
        index->draw(path);
    }

private:
    std::unique_ptr<KDTree> index;
    int64_t minimum_partition_size = 100;

    void initialize_index();
    void initialize_index_recursion(KDNode* current, int64_t lower_limit, int64_t upper_limit, int64_t column);

    std::pair<float, int64_t> find_median(int64_t column, int64_t lower_limit, int64_t upper_limit);

    // Returns the position on where the pivot would end
    int64_t pivot_table(int64_t column, int64_t low, int64_t high, float pivot, int64_t pivot_position);
};
#endif // MEDIAN_KD_TREE_H
