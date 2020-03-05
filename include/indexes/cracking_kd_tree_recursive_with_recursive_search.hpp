#ifndef CRACKING_KD_TREE_RECURSIVE_WITH_RECURSIVE_SEARCH_HPP
#define CRACKING_KD_TREE_RECURSIVE_WITH_RECURSIVE_SEARCH_HPP

#include "kd_tree/kd_tree_recursive.hpp"
#include "cracking_kd_tree.hpp"

class CrackingKDTreeRecursiveWithRecursiveSearch : public CrackingKDTree 
{
    public:
    static const size_t ID = 98;
    CrackingKDTreeRecursiveWithRecursiveSearch(std::map<std::string, std::string> config);
    ~CrackingKDTreeRecursiveWithRecursiveSearch();

    string name() override{
        return "CrackingKDTreeRecursiveWithRecursiveSearch";
    }

    void initialize(Table *table_to_copy) override;

    Table range_query(Query& query) override;

private:
    unique_ptr<KDTreeRecursive> index;
    void insert_point(Point& point, size_t is_right_hand_side) override;
    void insert_edge(Edge& edge) override;
    void insert_edge_recursion(
        KDNode *current,
        Edge& edge,
        int64_t lower_limit,
        int64_t upper_limit,
        size_t pivot_dim
    );
};

#endif
