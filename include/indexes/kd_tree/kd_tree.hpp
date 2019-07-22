#ifndef KDTREE
#define KDTREE

#include "../../helpers/query.hpp"
#include "kd_node.hpp"

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


    KDTree(size_t row_count);
    ~KDTree();

    vector<pair<size_t, size_t>> search(Query& query);

    shared_ptr<KDNode> create_node(size_t column, float key, size_t position);

    size_t get_node_count();

    size_t get_max_height();

    size_t get_min_height();

private:

    size_t number_of_nodes = 0;

    vector<pair<size_t, size_t>> partitions;
    vector<shared_ptr<KDNode>> nodes_to_check;
    vector<size_t> lower_limits, upper_limits;

    // Checks the left child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void get_partition_or_follow_left(shared_ptr<KDNode> current, size_t lower_limit);

    // Checks the right child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void get_partition_or_follow_right(shared_ptr<KDNode> current, size_t upper_limit);

    // Checks if node's column is inside of query
    bool node_in_query(shared_ptr<KDNode> current, Query& query);

    // If the node's key is greater or equal to the high part of the query
    // Then follow the left child
    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    bool node_greater_equal_query(shared_ptr<KDNode> node, Query& query);

    // If the node's key is smaller to the low part of the query
    // Then follow the right child
    //                  Key
    // Data:  |----------!--------|
    // Query:            |-----|
    //                  low   high
    bool node_less_equal_query(shared_ptr<KDNode> node, Query& query);
};

#endif // KDTREE