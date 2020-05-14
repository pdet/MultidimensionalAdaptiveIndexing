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
    size_t start;
    size_t current_start;
    size_t current_end;
    size_t end;

    KDNode(size_t column, float key, size_t position);

    KDNode(size_t column, float key, size_t start, size_t end) : key(key), column(column),
                                                                                 left_child(nullptr),
                                                                                 right_child(nullptr), position(0),
                                                                                 start(start),
                                                                                 current_start(start),
                                                                                 current_end(end),
                                                                                 end(end) {}

    KDNode(const KDNode &node);

    KDNode();

    ~KDNode();

    std::string label();

    int8_t compare(Query &query);
    void setLeft(std::unique_ptr<KDNode> child);
    void setRight(std::unique_ptr<KDNode> child);
    bool noChildren();
};

#endif // KDNODE_H
