#ifndef KDNODE_H
#define KDNODE_H

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <memory>
#include <string>
#include "query.hpp"

using namespace std;

class KDNode;

class KDNode
{
public:

    float key; // Search-key
    int64_t column; // Search column

    unique_ptr<KDNode> left_child;
    unique_ptr<KDNode> right_child;

    // Strict smaller than key from this position backwards
    // Greater or equal than key from this position forward.
    int64_t position; 

    KDNode(int64_t column, float key, int64_t position);
    KDNode(const KDNode &node);
    KDNode();
    ~KDNode();
    std::string label();

    // Checks if node is greater or equal than query 
    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    bool node_greater_equal_query(Query& query);

    // Checks if node is smaller than query 
    //                  Key
    // Data:  |----------!--------|
    // Query:            |-----|
    //                  low   high
    bool node_less_equal_query(Query& query);

};

#endif // KDNODE_H
