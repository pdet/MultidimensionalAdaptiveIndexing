#ifndef CRACKING_KD_TREE_RECURSIVE_HPP
#define CRACKING_KD_TREE_RECURSIVE_HPP

#include "kd_tree/kd_tree.hpp"
#include "cracking_kd_tree.hpp"

class CrackingKDTreeRecursive : public CrackingKDTree 
{
    public:
    static const size_t ID = 99;
    CrackingKDTreeRecursive(std::map<std::string, std::string> config);
    ~CrackingKDTreeRecursive();

    string name() override{
        return "CrackingKDTreeRecursive";
    }

private:
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
