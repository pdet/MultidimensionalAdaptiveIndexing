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

class KDNode {
public:

    float key; // Search-key
    size_t column; // Search column

    unique_ptr<KDNode> left_child;
    unique_ptr<KDNode> right_child;

    // Strict smaller than key from this position backwards
    // Greater or equal than key from this position forwards
    size_t position;

    //! Only used in progressive indexing
    size_t current_start;
    size_t current_end;
    //! If we finish sorting this piece
    bool finished;

    KDNode(size_t column, float key, size_t position);

    KDNode(size_t column, float key, size_t current_start, size_t current_end) : key(key), column(column),
                                                                                 left_child(nullptr),
                                                                                 right_child(nullptr), position(0),
                                                                                 current_start(current_start),
                                                                                 current_end(current_end),
                                                                                 finished(finished) {}

    KDNode(const KDNode &node);

    KDNode();

    ~KDNode();

    std::string label();

    int8_t compare(Query &query);

    bool noChildren();
};

#endif // KDNODE_H
