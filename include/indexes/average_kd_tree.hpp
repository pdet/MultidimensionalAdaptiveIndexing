#ifndef AVERAGE_KD_TREE_H
#define AVERAGE_KD_TREE_H

#include "kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>

class AverageKDTree : public AbstractIndex
{
public:
    static const size_t ID = 4;
    AverageKDTree(std::map<std::string, std::string> config);
    ~AverageKDTree();

    std::string name() override{
        return "AverageKDTree";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Table *originalTable,Query& query) override;

  std::unique_ptr<Table> range_query(Table *originalTable,Query& query) override;

    void draw_index(std::string path) override{
        index->draw(path);
    }

private:
    std::unique_ptr<KDTree> index;
    size_t minimum_partition_size = 100;

    void initialize_index();
    void initialize_index_recursion(KDNode* current, size_t lower_limit, size_t upper_limit, size_t column);

    pair<float, size_t> find_average(size_t column, size_t lower_limit, size_t upper_limit);
};
#endif // AVERAGE_KD_TREE_H
