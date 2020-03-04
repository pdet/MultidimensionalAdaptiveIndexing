#ifndef KDTREE_RECURSIVE_H
#define KDTREE_RECURSIVE_H

#include "../../helpers/query.hpp"
#include "kd_node.hpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <string>

using namespace std;

class KDTreeRecursive
{
public:

    unique_ptr<KDNode> root; // Root of the tree
    int64_t row_count;


    KDTreeRecursive(int64_t row_count);
    ~KDTreeRecursive();

    vector<pair<int64_t, int64_t>> search(Query& query);

    unique_ptr<KDNode> create_node(int64_t column, float key, int64_t position);

    int64_t get_node_count();

    int64_t get_max_height();

    int64_t get_min_height();

    void draw(std::string path);

private:

    int64_t number_of_nodes = 0;

    void search_recursion(
        KDNode *current,
        int64_t lower_limit,
        int64_t upper_limit,
        Query& query,
        vector<pair<int64_t, int64_t>> &partitions
    );
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

#endif // KDTREE_RECURSIVE_H
