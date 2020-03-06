#include "kd_node.hpp"
#include "cracking_kd_tree_mine.hpp"

CrackingKDTreeMine::CrackingKDTreeMine(std::map<std::string, std::string> config) : CrackingKDTree(config){}
CrackingKDTreeMine::~CrackingKDTreeMine(){}

void CrackingKDTreeMine::insert_edge(CrackingKDTreeMine::Edge& edge){
    // Determine which dimension value is fixed
    // (x1, y1) - (x2, y1)
    // Here y1 is fixed, which means this will be the pivot
    
    auto n_dimensions = edge.first.size();
    size_t pivot_dim = 0;
    for(size_t i = 0; i < n_dimensions; ++i){
        if(edge.first.at(i) == edge.second.at(i)){
            pivot_dim = i;
            break;
        }
    }

    // Iterate over the tree to find the partitions where to insert the edge
    MyStack<KDNode*> nodes_to_check;
    MyStack<int64_t> lower_limits;
    MyStack<int64_t> upper_limits;
    nodes_to_check.push_back(index->root.get());
    lower_limits.push_back(0);
    upper_limits.push_back(table->row_count());
    while(!nodes_to_check.empty()){
        auto current = nodes_to_check.pop_back();

        auto lower_limit = lower_limits.pop_back();

        auto upper_limit = upper_limits.pop_back();

        // If the size of the partition is already too small then stop exploring it
        if(upper_limit - lower_limit + 1 < minimum_partition_size)
            continue;

        // If that pivot has already been inserted then we don't need to
        // keep looking in this branch.
        if(current->column == pivot_dim && current->key == edge.first.at(pivot_dim)){
            continue;
        }

        //                  Key
        // Data:  |----------!--------|
        // Query:      |-----|
        //            low   high
        if(max(edge.first, edge.second, current->column) <= current->key){
            if(current->left_child == nullptr){
                auto position = table->CrackTable(
                        lower_limit, current->position,
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->left_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
                        );
                continue;
            }
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->position);
            nodes_to_check.push_back(current->left_child.get());
        }
        //                  Key
        // Data:  |----------!--------|
        // Query:            |-----|
        //                  low   high
        else if(current->key <= min(edge.first, edge.second, current->column)){
            if(current->right_child == nullptr){
                auto position = table->CrackTable(
                        current->position, upper_limit,
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->right_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
                        );
                continue;
            }
            lower_limits.push_back(current->position);
            upper_limits.push_back(upper_limit);
            nodes_to_check.push_back(current->right_child.get());

        }
        //                  Key
        // Data:  |----------!--------|
        // Query:         |-----|
        //               low   high
        else{
            if(current->left_child == nullptr){
                auto position = table->CrackTable(
                        lower_limit, current->position,
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->left_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
                        );
                continue;
            }
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->position);
            nodes_to_check.push_back(current->left_child.get());

            if(current->right_child == nullptr){
                auto position = table->CrackTable(
                        current->position, upper_limit,
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->right_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
                        );
                continue;
            }
            lower_limits.push_back(current->position);
            upper_limits.push_back(upper_limit);
            nodes_to_check.push_back(current->right_child.get());
        }

    }

}
