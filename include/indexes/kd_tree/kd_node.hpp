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
    // Greater or equal than key from this position forwards
    int64_t position;

    //! Only used in progressive indexing
    size_t current_start;
    size_t current_end;
    //! If we finish sorting this piece
    bool sorted;

    KDNode(int64_t column, float key, int64_t position);
    KDNode(const KDNode &node);
    KDNode();
    ~KDNode();
    std::string label();
    int8_t compare(Query& query);
    bool noChildren();
};

#endif // KDNODE_H
