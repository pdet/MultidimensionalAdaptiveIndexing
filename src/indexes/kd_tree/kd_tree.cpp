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

void KDTree::search_recursion(
    KDNode *current,
    int64_t lower_limit,
    int64_t upper_limit,
    Query& query,
    vector<pair<int64_t, int64_t>> &partitions,
    vector<bool> &partition_skip,
    vector<pair<float, float>> partition_borders
){
    auto temporary_min = partition_borders.at(current->column).first;
    switch(current->compare(query)){
        case -1:
            // If the node's key is smaller to the low part of the query
            // Then follow the right child
            //                  Key
            // Data:  |----------!--------|
            // Query:            |-----|
            //                  low   high
            if(current->right_child == nullptr){
                partitions.push_back(make_pair(current->position, upper_limit));
                partition_skip.push_back(
                        query.covers(partition_borders)
                        );
            }
            else{
                partition_borders.at(current->column).first = current->key;
                search_recursion(
                        current->right_child.get(),
                        current->position, upper_limit,
                        query, partitions, partition_skip,
                        partition_borders
                        );
            }
            break; 
        case +1:
            // If the node's key is greater or equal to the high part of the query
            // Then follow the left child
            //                  Key
            // Data:  |----------!--------|
            // Query:      |-----|
            //            low   high
            if(current->left_child == nullptr){
                partitions.push_back(make_pair(lower_limit, current->position));
                partition_skip.push_back(
                        query.covers(partition_borders)
                        );
            }
            else{
                partition_borders.at(current->column).second = current->key;
                search_recursion(
                        current->left_child.get(),
                        lower_limit, current->position,
                        query, partitions, partition_skip,
                        partition_borders
                        );
            }
            break;
        case 0:
            // If the node's key is inside the query
            // Then follow both children
            //                  Key
            // Data:  |----------!--------|
            // Query:         |-----|
            //               low   high
            if(current->left_child == nullptr){
                partitions.push_back(make_pair(lower_limit, current->position));
                partition_skip.push_back(
                        query.covers(partition_borders)
                        );
            }
            else{
                partition_borders.at(current->column).first = current->key;
                search_recursion(
                        current->left_child.get(),
                        lower_limit, current->position,
                        query, partitions, partition_skip,
                        partition_borders
                        );
            }
            if(current->right_child == nullptr){
                partitions.push_back(make_pair(current->position, upper_limit));
                partition_skip.push_back(
                        query.covers(partition_borders)
                        );
            }
            else{
                partition_borders.at(current->column).first = temporary_min;
                partition_borders.at(current->column).second= current->key;
                search_recursion(
                        current->right_child.get(),
                        current->position, upper_limit,
                        query, partitions, partition_skip,
                        partition_borders
                        );
            }
            break;
        default:
            assert(false);
            break;
    }
}

pair<vector<pair<int64_t, int64_t>>, vector<bool>>
KDTree::search(Query& query){
    vector<pair<int64_t, int64_t>> partitions;
    vector<bool> partition_skip;
    if(root == nullptr){
        partitions.push_back(
            make_pair(0, row_count-1)
        );
        partition_skip.push_back(false);
        return make_pair(partitions, partition_skip);
    }

    vector<pair<float, float>> partition_borders(query.predicate_count());
    for(size_t i = 0; i < query.predicate_count(); ++i){
        partition_borders.at(i) = make_pair(
                numeric_limits<float>::lowest(),
                numeric_limits<float>::max()
                );
    }
    search_recursion(
        root.get(),
        0, row_count,
        query, partitions, partition_skip,
        partition_borders
        );
    return make_pair(partitions, partition_skip);
}

unique_ptr<KDNode> KDTree::create_node(int64_t column, float key, int64_t position){
    auto node = make_unique<KDNode>(
                column, key, position 
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

            myfile << std::to_string(reinterpret_cast<size_t>(node));
            myfile << "[label=\"" + node->label() + "\"";
            // myfile << ", style=filled, fillcolor=" + colors[node->column];
            myfile << "]\n;"; 

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
