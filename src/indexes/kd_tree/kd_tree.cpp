#include "kd_tree.hpp"
#include "kd_node.hpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <fstream>
#include <map>
#include "full_scan.hpp"

using namespace std;

KDTree::KDTree(size_t row_count) : root(nullptr), row_count(row_count) {}

KDTree::~KDTree() {}

void KDTree::search_recursion(
        KDNode *current,
        size_t lower_limit,
        size_t upper_limit,
        Query &query,
        vector<pair<size_t, size_t>> &partitions,
        vector<bool> &partition_skip,
        vector<pair<float, float>> partition_borders
) {
    // Progressive
    //! TODO: there is some space for optimization here if necessary
    if (current->current_start < current->current_end) {
        switch (current->compare(query)) {
            case -1:
                //! Key < Query
                partitions.push_back(make_pair(current->current_start, current->end + 1));
                partition_skip.push_back(false);
                break;
            case +1:
                //! Key >= Query
                partitions.push_back(make_pair(current->start, current->current_end + 1));
                partition_skip.push_back(false);
                break;
            case 0:
                //! Key doesn't really help
                partitions.push_back(make_pair(current->start, current->end + 1));
                partition_skip.push_back(false);
                break;
            default:
                assert(false);
        }
        return;
    }
    switch (current->compare(query)) {
        case -1:
            // If the node's key is smaller to the low part of the query
            // Then follow the right child
            //                  Key
            // Data:  |----------!--------|
            // Query:            |-----|
            //                  low   high
            if (current->right_child == nullptr) {
                partitions.push_back(make_pair(current->position, upper_limit));
                partition_skip.push_back(
                        query.covers(partition_borders)
                );
            } else {
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
            if (current->left_child == nullptr) {
                partitions.push_back(make_pair(lower_limit, current->position));
                partition_skip.push_back(
                        query.covers(partition_borders)
                );
            } else {
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
            if (current->left_child == nullptr) {
                partitions.push_back(make_pair(lower_limit, current->position));
                partition_skip.push_back(
                        query.covers(partition_borders)
                );
            } else {
                auto tmp = partition_borders.at(current->column).second;
                partition_borders.at(current->column).second = current->key;
                search_recursion(
                        current->left_child.get(),
                        lower_limit, current->position,
                        query, partitions, partition_skip,
                        partition_borders
                );
                partition_borders.at(current->column).second = tmp;
            }
            if (current->right_child == nullptr) {
                partitions.push_back(make_pair(current->position, upper_limit));
                partition_skip.push_back(
                        query.covers(partition_borders)
                        );
            } else {
                partition_borders.at(current->column).first = current->key;
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

pair<vector<pair<size_t, size_t>>, vector<bool>>
KDTree::search(Query &query) {
    vector<pair<size_t, size_t>> partitions;
    vector<bool> partition_skip;
    if (root == nullptr) {
        partitions.push_back(
                make_pair(0u, row_count - 1u)
        );
        partition_skip.push_back(false);
        return make_pair(partitions, partition_skip);
    }

    vector<pair<float, float>> partition_borders(query.predicate_count());
    for (size_t i = 0; i < query.predicate_count(); ++i) {
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

void KDTree::search_nodes_recursion(
        KDNode *current,
        size_t lower_limit,
        size_t upper_limit,
        Query &query, vector<KDNode *> &nodes
) {
    if (current->current_start < current->current_end) {
        switch (current->compare(query)) {
            case -1:
                //! Key < Query
                nodes.push_back(current);
                break;
            case +1:
                //! Key >= Query
                nodes.push_back(current);
                break;
            case 0:
                //! Key doesn't really help
                break;
            default:
                assert(false);
        }
        return;
    }
    switch (current->compare(query)) {
        case -1:
            // If the node's key is smaller to the low part of the query
            // Then follow the right child
            //                  Key
            // Data:  |----------!--------|
            // Query:            |-----|
            //                  low   high

            if (current->right_child != nullptr) {
                search_nodes_recursion(
                        current->right_child.get(),
                        current->position, upper_limit,
                        query, nodes
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
            if (current->left_child != nullptr) {
                search_nodes_recursion(
                        current->left_child.get(),
                        lower_limit, current->position,
                        query, nodes
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
            if (current->left_child != nullptr) {
                search_nodes_recursion(
                        current->left_child.get(),
                        lower_limit, current->position,
                        query, nodes
                );
            }
            if (current->right_child != nullptr) {
                search_nodes_recursion(
                        current->right_child.get(),
                        current->position, upper_limit,
                        query, nodes
                );
            }
            break;
        default:
            assert(false);
    }
}

vector<KDNode *>
KDTree::search_nodes(Query &query, vector<KDNode *> &nodes) {
    search_nodes_recursion(
            root.get(),
            0, row_count,
            query, nodes
    );
    return nodes;
}

unique_ptr<KDNode> KDTree::create_node(size_t column, float key, size_t position) {
    auto node = make_unique<KDNode>(
            column, key, position
    );
    number_of_nodes++;
    return node;
}

size_t KDTree::get_node_count() {
    return number_of_nodes;
}

size_t KDTree::get_max_height() {
    if (root == nullptr)
        return 0;
    vector<KDNode *> nodes;
    vector<size_t> heights;

    size_t max_height = 0;

    nodes.push_back(root.get());
    heights.push_back(1);

    while (!nodes.empty()) {
        auto node = nodes.back();
        nodes.pop_back();

        auto height = heights.back();
        heights.pop_back();

        if (node->left_child.get() != nullptr) {
            nodes.push_back(node->left_child.get());
            heights.push_back(height + 1);
        }

        if (node->right_child.get() != nullptr) {
            nodes.push_back(node->right_child.get());
            heights.push_back(height + 1);
        }

        if (node->left_child.get() == nullptr && node->right_child.get() == nullptr) {
            if (max_height < height)
                max_height = height;
        }
    }

    return max_height;
}

size_t KDTree::get_min_height() {
    if (root == nullptr)
        return 0;

    vector<KDNode *> nodes;
    vector<size_t> heights;

    size_t min_height = numeric_limits<size_t>::max();

    nodes.push_back(root.get());
    heights.push_back(1);

    while (!nodes.empty()) {
        auto node = nodes.back();
        nodes.pop_back();

        auto height = heights.back();
        heights.pop_back();

        if (node->left_child.get() != nullptr) {
            nodes.push_back(node->left_child.get());
            heights.push_back(height + 1);
        }

        if (node->right_child.get() != nullptr) {
            nodes.push_back(node->right_child.get());
            heights.push_back(height + 1);
        }

        if (node->left_child.get() == nullptr && node->right_child.get() == nullptr) {
            if (min_height > height)
                min_height = height;
        }
    }

    return min_height;
}

void KDTree::draw(std::string path) {
    std::ofstream myfile(path.c_str());

    myfile << "digraph KDTree {\n";

    std::map<size_t, std::string> labels;
    if (root != nullptr) {

        vector<KDNode *> nodes;
        vector<size_t> heights;
        size_t n_nulls = 0;
        nodes.push_back(root.get());

        while (!nodes.empty()) {
            auto node = nodes.back();
            nodes.pop_back();

            myfile << std::to_string(reinterpret_cast<size_t>(node));
            myfile << "[label=\"" + node->label() + "\"";
            // myfile << ", style=filled, fillcolor=" + colors[node->column];
            myfile << "]\n;";

            if (node->left_child.get() != nullptr) {
                nodes.push_back(node->left_child.get());
                myfile << std::to_string(reinterpret_cast<size_t>(node));
                myfile << " -> ";
                myfile << std::to_string(
                        reinterpret_cast<size_t>(node->left_child.get())
                );
                myfile << "[label =\"L\"];\n";
            } else {
                myfile << std::to_string(reinterpret_cast<size_t>(node));
                myfile << " -> ";
                myfile << "null" + std::to_string(n_nulls);
                myfile << "[label =\"L\"];\n";
                myfile << "null" + std::to_string(n_nulls) + "[shape=point]\n";
                n_nulls++;
            }
            if (node->right_child.get() != nullptr) {
                nodes.push_back(node->right_child.get());
                myfile << std::to_string(reinterpret_cast<size_t>(node));
                myfile << " -> ";
                myfile << std::to_string(
                        reinterpret_cast<size_t>(node->right_child.get())
                );
                myfile << "[label =\"R\"];\n";
            } else {
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

bool KDTree::sanity_check_recursion(
        Table *table, KDNode *current,
        size_t low, size_t high,
        vector<pair<float, float>> partition_borders,
        vector<pair<size_t, size_t>> &partitions
) {
    if (current == nullptr) {
        // Scan Partition
        // Transform partition_borders to query
        auto query = Query(partition_borders);
        // Scan using Full Scan
        partitions.push_back(make_pair(low, high));
        vector<pair<size_t, size_t>> partition {partitions.back()};

        vector<bool> skip(1, false);

        auto result = FullScan::scan_partition(table, query, partition, skip);
        // Check if the number of returned tuples is equal to the number
        //  of tuples in the partition
        return result.second == (high - low);
    }

    auto temporary_max = partition_borders.at(current->column).second;

    partition_borders.at(current->column).second = current->key;
    auto left_sanity = sanity_check_recursion(
            table, current->left_child.get(),
            low, current->position,
            partition_borders,
            partitions
    );

    partition_borders.at(current->column).first = current->key;
    partition_borders.at(current->column).second = temporary_max;
    auto right_sanity = sanity_check_recursion(
            table, current->right_child.get(),
            current->position, high,
            partition_borders,
            partitions
    );
    return left_sanity && right_sanity;
}

bool KDTree::sanity_check(Table *table) {
    vector<pair<float, float>> partition_borders(table->col_count());
    for (size_t i = 0; i < table->col_count(); ++i) {
        partition_borders.at(i) = make_pair(
                numeric_limits<float>::lowest(),
                numeric_limits<float>::max()
        );
    }

    vector<pair<size_t, size_t>> partitions;
    auto cond1 = sanity_check_recursion(table, root.get(), 0, row_count, partition_borders, partitions);

    size_t tuples = 0;
    for(auto& p : partitions){
        tuples += p.second - p.first;
    }

    bool cond2 = tuples == table->row_count();

    return cond1 && cond2;
}
