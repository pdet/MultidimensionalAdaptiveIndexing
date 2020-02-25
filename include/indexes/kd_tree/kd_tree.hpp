#ifndef KDTREE
#define KDTREE

#include "../../helpers/query.hpp"
#include "kd_node.hpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <string>

using namespace std;

class KDTree
{
public:

    unique_ptr<KDNode> root; // Root of the tree
    int64_t row_count;


    KDTree(int64_t row_count);
    ~KDTree();

    vector<pair<int64_t, int64_t>> search(Query& query);

    unique_ptr<KDNode> create_node(int64_t column, float key, int64_t position);

    int64_t get_node_count();

    int64_t get_max_height();

    int64_t get_min_height();

    void draw(std::string path);

private:

    int64_t number_of_nodes = 0;

    vector<pair<int64_t, int64_t>> partitions;
    vector<KDNode*> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;

    // Checks the left child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void get_partition_or_follow_left(KDNode *current, int64_t lower_limit);

    // Checks the right child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void get_partition_or_follow_right(KDNode *current, int64_t upper_limit);

    // If the node's key is greater or equal to the high part of the query
    // Then follow the left child
    // This behavior changes if low == high in query, which means it is a equality
    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    bool node_greater_equal_query(KDNode *node, Query& query);

    // If the node's key is smaller to the low part of the query
    // Then follow the right child
    //                  Key
    // Data:  |----------!--------|
    // Query:            |-----|
    //                  low   high
    bool node_less_equal_query(KDNode *node, Query& query);
};

#endif // KDTREE
