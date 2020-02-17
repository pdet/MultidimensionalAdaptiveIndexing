#include "kd_tree.hpp"
#include "kd_node.hpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <fstream>
#include <map>

using namespace std;

KDTree::KDTree(int64_t row_count) : row_count(row_count){
    root = nullptr;
}
KDTree::~KDTree(){}

vector<pair<int64_t, int64_t>> KDTree::search(Query& query){
    partitions.resize(0);

    if(root == nullptr){
        partitions.push_back(
            make_pair(0, row_count-1)
        );
        return partitions;
    }
    nodes_to_check.resize(0);
    lower_limits.resize(0);
    upper_limits.resize(0);

    nodes_to_check.push_back(root.get());
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

unique_ptr<KDNode> KDTree::create_node(int64_t column, float key, int64_t position){
    auto node = make_unique<KDNode>(
                column, key, position, position + 1
            );
    number_of_nodes++;
    return node;
}

int64_t KDTree::get_node_count(){
    return number_of_nodes;
}

int64_t KDTree::get_max_height(){
    if(root == nullptr)
        return 0;
    vector<KDNode*> nodes;
    vector<int64_t> heights;

    int64_t max_height = 0;

    nodes.push_back(root.get());
    heights.push_back(1);

    while(!nodes.empty()){
        auto node = nodes.back();
        nodes.pop_back();

        auto height = heights.back();
        heights.pop_back();

        if(node->left_child.get() != nullptr){
            nodes.push_back(node->left_child.get());
            heights.push_back(height + 1);
        }

        if(node->right_child.get() != nullptr){
            nodes.push_back(node->right_child.get());
            heights.push_back(height + 1);
        }

        if(node->left_child.get() == nullptr && node->right_child.get() == nullptr){
            if(max_height < height)
                max_height = height;
        }
    }

    return max_height;
}

int64_t KDTree::get_min_height(){
    if(root == nullptr)
        return 0;

    vector<KDNode*> nodes;
    vector<int64_t> heights;

    int64_t min_height = numeric_limits<int64_t>::max();

    nodes.push_back(root.get());
    heights.push_back(1);

    while(!nodes.empty()){
        auto node = nodes.back();
        nodes.pop_back();

        auto height = heights.back();
        heights.pop_back();

        if(node->left_child.get() != nullptr){
            nodes.push_back(node->left_child.get());
            heights.push_back(height + 1);
        }

        if(node->right_child.get() != nullptr){
            nodes.push_back(node->right_child.get());
            heights.push_back(height + 1);
        }

        if(node->left_child.get() == nullptr && node->right_child.get() == nullptr){
            if(min_height > height)
                min_height = height;
        }
    }

    return min_height;
}

void KDTree::draw(std::string path){
    std::ofstream myfile(path.c_str());

    myfile << "digraph KDTree {\n";

    std::map<size_t, std::string> labels;
    if(root != nullptr){

        vector<KDNode*> nodes;
        vector<int64_t> heights;
        size_t n_nulls = 0;
        nodes.push_back(root.get());

        while(!nodes.empty()){
            auto node = nodes.back();
            nodes.pop_back();

            myfile << std::to_string(reinterpret_cast<size_t>(node))\
                   << "[label=\"" + node->label() + "\"]\n;"; 

            if(node->left_child.get() != nullptr){
                nodes.push_back(node->left_child.get());
                myfile << std::to_string(reinterpret_cast<size_t>(node));
                myfile << " -> ";
                myfile << std::to_string(
                    reinterpret_cast<size_t>(node->left_child.get())
                );
                myfile << "[label =\"L\"];\n";
            }
            else{
                myfile << std::to_string(reinterpret_cast<size_t>(node));
                myfile << " -> ";
                myfile << "null" + std::to_string(n_nulls);
                myfile << "[label =\"L\"];\n";
                myfile << "null" + std::to_string(n_nulls) + "[shape=point]\n";
                n_nulls++;
            }
            if(node->right_child.get() != nullptr){
                nodes.push_back(node->right_child.get());
                myfile << std::to_string(reinterpret_cast<size_t>(node));
                myfile << " -> ";
                myfile << std::to_string(
                    reinterpret_cast<size_t>(node->right_child.get())
                );
                myfile << "[label =\"R\"];\n";
            }
            else{
                myfile << std::to_string(reinterpret_cast<size_t>(node));
                myfile << " -> ";
                myfile << "null" + std::to_string(n_nulls);
                myfile << "[label =\"R\"];\n";
                myfile << "null" + std::to_string(n_nulls) + "[shape=point]\n";
                n_nulls++;
            }

        }

    }
    myfile << "\n}";
    myfile.close();
}

// Checks the left child
// If it is null then we reached a partition
// Otherwise, we follow it
void KDTree::get_partition_or_follow_left(KDNode *current, int64_t lower_limit){
    if (current->left_child == nullptr)
    {
        partitions.push_back(make_pair(lower_limit, current->left_position));
    }
    else
    {
        nodes_to_check.push_back(current->left_child.get());
        lower_limits.push_back(lower_limit);
        upper_limits.push_back(current->left_position);
    }
}

// Checks the right child
// If it is null then we reached a partition
// Otherwise, we follow it
void KDTree::get_partition_or_follow_right(KDNode *current, int64_t upper_limit){
    if (current->right_child == nullptr)
    {
        partitions.push_back(make_pair(current->right_position, upper_limit));
    }
    else
    {
        nodes_to_check.push_back(current->right_child.get());
        lower_limits.push_back(current->right_position);
        upper_limits.push_back(upper_limit);
    }
}

// Checks if node's column is inside of query
bool KDTree::node_in_query(KDNode *current, Query& query){
    for(int64_t i = 0; i < query.predicate_count(); i++)
    {
        if(current->column == query.predicates.at(i).column)
            return true;
    }
    return false;
}

// If the node's key is greater or equal to the high part of the query
// Then follow the left child
// This behavior changes if low == high in query, which means it is a equality
//                  Key
// Data:  |----------!--------|
// Query:      |-----|
//            low   high
bool KDTree::node_greater_equal_query(KDNode *node, Query& query){
    for(int64_t i = 0; i < query.predicate_count(); i++)
    {
        if(node->column == query.predicates.at(i).column){
            auto high = query.predicates.at(i).high;
            auto low = query.predicates.at(i).low;
            if(low != high)
                return high <= node->key;
            else
                return high < node->key;
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
bool KDTree::node_less_equal_query(KDNode *node, Query& query){
    for(int64_t i = 0; i < query.predicate_count(); i++)
    {
        if(node->column == query.predicates.at(i).column){
            auto low = query.predicates.at(i).low;
            return node->key <= low;
        }
    }
    return false;
}
