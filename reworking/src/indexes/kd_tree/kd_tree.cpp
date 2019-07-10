#ifndef KDTREE
#define KDTREE

#include "../../helpers/helpers.cpp"
#include "kd_node.cpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>

using namespace std;

class KDTree
{
public:

    unique_ptr<KDNode> root; // Root of the tree
    size_t row_count;


    KDTree(size_t row_count) : row_count(row_count){
        root = nullptr;
    }
    ~KDTree(){}

    vector<pair<size_t, size_t>> search(shared_ptr<Query> query, size_t number_of_rows){
        partitions.resize(0);
        nodes_to_check.resize(0);
        lower_limits.resize(0);
        upper_limits.resize(0);

        nodes_to_check.push_back(make_unique<KDNode>(root));
        lower_limits.push_back(0);
        upper_limits.push_back(number_of_rows - 1);
        while(!nodes_to_check.empty()){
            unique_ptr<KDNode> current = move(nodes_to_check.back());
            nodes_to_check.pop_back();

            int64_t lower_limit = lower_limits.back();
            lower_limits.pop_back();

            int64_t upper_limit = upper_limits.back();
            upper_limits.pop_back();

            // If current's column is not in query follow both children
            if(!node_in_query(move(current), query)){
                get_partition_or_follow_left(move(current), lower_limit);
                get_partition_or_follow_right(move(current), upper_limit);
            }
            // If the node's key is greater or equal to the high part of the query
            // Then follow the left child
            //                  Key
            // Data:  |----------!--------|
            // Query:      |-----|
            //            low   high
            else if (node_greater_equal_query(move(current), query)){
                get_partition_or_follow_left(move(current), lower_limit);
            }
            // If the node's key is smaller to the low part of the query
            // Then follow the right child
            //                  Key
            // Data:  |----------!--------|
            // Query:            |-----|
            //                  low   high
            else if (node_less_equal_query(move(current), query)){
                get_partition_or_follow_right(move(current), upper_limit);
            }
            // If the node's key is inside the query
            // Then follow both children
            //                  Key
            // Data:  |----------!--------|
            // Query:         |-----|
            //               low   high
            else{
                get_partition_or_follow_left(move(current), lower_limit);
                get_partition_or_follow_right(move(current), upper_limit);
            }
        }
        return partitions;
    }

    void insert(shared_ptr<Query> query){
        // TODO: implement insertion using the entire query
    }

    size_t memory_footprint(){
        return number_of_nodes * sizeof(KDNode);
    }

    size_t node_count(){
        return number_of_nodes;
    }

    size_t max_height(){
        return height;
    }

private:

    size_t number_of_nodes;
    size_t height;

    vector<pair<size_t, size_t>> partitions;
    vector<unique_ptr<KDNode>> nodes_to_check;
    vector<size_t> lower_limits, upper_limits;

    // Checks the left child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void get_partition_or_follow_left(unique_ptr<KDNode> current, size_t lower_limit){
        if (current->left_child == nullptr)
        {
            partitions.push_back(make_pair(lower_limit, current->left_position));
        }
        else
        {
            nodes_to_check.push_back(current->left_child);
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->left_position);
        }
    }

    // Checks the right child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void get_partition_or_follow_right(unique_ptr<KDNode> current, size_t upper_limit){
        if (current->right_child == nullptr)
        {
            partitions.push_back(make_pair(current->right_position, upper_limit));
        }
        else
        {
            nodes_to_check.push_back(current->right_child);
            lower_limits.push_back(current->right_position);
            upper_limits.push_back(upper_limit);
        }
    }

    // Checks if node's column is inside of query
    bool node_in_query(unique_ptr<KDNode> current, shared_ptr<Query> query){
        bool inside = false;
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(current->column == query->predicates.at(i)->column)
                inside = true;
        }
        return inside;
    }

    // If the node's key is greater or equal to the high part of the query
    // Then follow the left child
    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    bool node_greater_equal_query(unique_ptr<KDNode> node, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(node->column == query->predicates.at(i)->column){
                int64_t high = query->predicates.at(i)->high;
                return high <= node->key;
            }
        }
        return false;
    }

    // If the node's key is smaller to the low part of the query
    // Then follow the right child
    //                  Key
    // Data:  |----------!--------|
    // Query:            |-----|
    //                  low   high
    bool node_less_equal_query(unique_ptr<KDNode> node, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(node->column == query->predicates.at(i)->column){
                int64_t low = query->predicates.at(i)->low;
                return node->key <= low;
            }
        }
        return false;
    }
};

#endif // KDTREE