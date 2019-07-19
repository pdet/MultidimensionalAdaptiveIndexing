#ifndef KDTREE
#define KDTREE

#include "../../helpers/helpers.cpp"
#include "kd_node.cpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>

using namespace std;

class KDTree
{
public:

    shared_ptr<KDNode> root; // Root of the tree
    size_t row_count;


    KDTree(size_t row_count) : row_count(row_count){
        root = nullptr;
    }
    ~KDTree(){}

    vector<pair<size_t, size_t>> search(shared_ptr<Query> query){
        partitions.resize(0);

        if(root == nullptr){
            partitions.push_back(
                make_pair(0, row_count)
            );
            return partitions;
        }
        nodes_to_check.resize(0);
        lower_limits.resize(0);
        upper_limits.resize(0);

        nodes_to_check.push_back(root);
        lower_limits.push_back(0);
        upper_limits.push_back(row_count - 1);
        while(!nodes_to_check.empty()){
            auto current = nodes_to_check.back();
            nodes_to_check.pop_back();

            auto lower_limit = lower_limits.back();
            lower_limits.pop_back();

            auto upper_limit = upper_limits.back();
            upper_limits.pop_back();

            // If current's column is not in query follow both children
            if(!node_in_query(current, query)){
                get_partition_or_follow_left(current, lower_limit);
                get_partition_or_follow_right(current, upper_limit);
            }
            // If the node's key is greater or equal to the high part of the query
            // Then follow the left child
            //                  Key
            // Data:  |----------!--------|
            // Query:      |-----|
            //            low   high
            else if (node_greater_equal_query(current, query)){
                get_partition_or_follow_left(current, lower_limit);
            }
            // If the node's key is smaller to the low part of the query
            // Then follow the right child
            //                  Key
            // Data:  |----------!--------|
            // Query:            |-----|
            //                  low   high
            else if (node_less_equal_query(current, query)){
                get_partition_or_follow_right(current, upper_limit);
            }
            // If the node's key is inside the query
            // Then follow both children
            //                  Key
            // Data:  |----------!--------|
            // Query:         |-----|
            //               low   high
            else{
                get_partition_or_follow_left(current, lower_limit);
                get_partition_or_follow_right(current, upper_limit);
            }
        }
        return partitions;
    }

    size_t get_max_height(){
        if(root == nullptr)
            return 0;
        vector<shared_ptr<KDNode>> nodes;
        vector<size_t> heights;

        size_t max_height = 0;

        nodes.push_back(root);
        heights.push_back(1);

        while(!nodes.empty()){
            auto node = nodes.back();
            nodes.pop_back();

            auto height = heights.back();
            heights.pop_back();

            if(node->left_child != nullptr){
                nodes.push_back(node->left_child);
                heights.push_back(height + 1);
            }

            if(node->right_child != nullptr){
                nodes.push_back(node->right_child);
                heights.push_back(height + 1);
            }

            if(node->left_child == nullptr && node->right_child == nullptr){
                if(max_height < height)
                    max_height = height;
            }
        }

        return max_height;
    }

    size_t get_min_height(){
        if(root == nullptr)
            return 0;
        vector<shared_ptr<KDNode>> nodes;
        vector<size_t> heights;

        size_t min_height = numeric_limits<size_t>::max();

        nodes.push_back(root);
        heights.push_back(1);

        while(!nodes.empty()){
            auto node = nodes.back();
            nodes.pop_back();

            auto height = heights.back();
            heights.pop_back();

            if(node->left_child != nullptr){
                nodes.push_back(node->left_child);
                heights.push_back(height + 1);
            }

            if(node->right_child != nullptr){
                nodes.push_back(node->right_child);
                heights.push_back(height + 1);
            }

            if(node->left_child == nullptr && node->right_child == nullptr){
                if(min_height > height)
                    min_height = height;
            }
        }

        return min_height;
    }

private:

    vector<pair<size_t, size_t>> partitions;
    vector<shared_ptr<KDNode>> nodes_to_check;
    vector<size_t> lower_limits, upper_limits;

    // Checks the left child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void get_partition_or_follow_left(shared_ptr<KDNode> current, size_t lower_limit){
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
    void get_partition_or_follow_right(shared_ptr<KDNode> current, size_t upper_limit){
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
    bool node_in_query(shared_ptr<KDNode> current, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(current->column == query->predicates.at(i)->column)
                return true;
        }
        return false;
    }

    // If the node's key is greater or equal to the high part of the query
    // Then follow the left child
    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    bool node_greater_equal_query(shared_ptr<KDNode> node, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(node->column == query->predicates.at(i)->column){
                auto high = query->predicates.at(i)->high;
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
    bool node_less_equal_query(shared_ptr<KDNode> node, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(node->column == query->predicates.at(i)->column){
                auto low = query->predicates.at(i)->low;
                return node->key <= low;
            }
        }
        return false;
    }
};

#endif // KDTREE