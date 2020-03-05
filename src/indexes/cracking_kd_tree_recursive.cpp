#include "kd_node.hpp"
#include "cracking_kd_tree_recursive.hpp"

CrackingKDTreeRecursive::CrackingKDTreeRecursive(std::map<std::string, std::string> config)
: CrackingKDTree(config){}
CrackingKDTreeRecursive::~CrackingKDTreeRecursive(){}

void CrackingKDTreeRecursive::insert_edge_recursion(
        KDNode *current,
        CrackingKDTreeRecursive::Edge& edge,
        int64_t lower_limit,
        int64_t upper_limit,
        size_t pivot_dim
        ){
    // If the size of the partition is already too small then stop exploring it
    if(upper_limit - lower_limit + 1 < minimum_partition_size)
        return;

    // If that pivot has already been inserted then we don't need to
    // keep looking in this branch.
    if(current->column == pivot_dim && current->key == edge.first.at(pivot_dim)){
        return;
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
            return;
        }
        insert_edge_recursion(
                current->left_child.get(),
                edge, lower_limit, current->position, pivot_dim
                );
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
            return;
        }
        insert_edge_recursion(
                current->right_child.get(),
                edge, current->position, upper_limit, pivot_dim
                );
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
            return;
        }
        insert_edge_recursion(
                current->left_child.get(),
                edge, lower_limit, current->position, pivot_dim
                );

        if(current->right_child == nullptr){
            auto position = table->CrackTable(
                    current->position, upper_limit,
                    edge.first.at(pivot_dim), pivot_dim 
                    );
            current->right_child = index->create_node(
                    pivot_dim, edge.first.at(pivot_dim), position
                    );
            return;
        }
        insert_edge_recursion(
                current->right_child.get(),
                edge, current->position, upper_limit, pivot_dim
                );
    }

}

void CrackingKDTreeRecursive::insert_edge(CrackingKDTreeRecursive::Edge& edge){
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
    insert_edge_recursion(index->root.get(), edge, 0, table->row_count(), pivot_dim);
}
