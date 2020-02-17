#ifndef CRACKING_KD_TREE_HPP
#define CRACKING_KD_TREE_HPP

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>

class CrackingKDTree : public AbstractIndex
{
    public:
    CrackingKDTree(std::map<std::string, std::string> config);
    ~CrackingKDTree();

    string name() override{
        return "CrackingKDTree";
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
};

#endif
