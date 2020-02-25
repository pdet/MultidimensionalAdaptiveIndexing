#ifndef KDNODE_H
#define KDNODE_H

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <memory>
#include <string>

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

    KDNode(int64_t column, float key, int64_t position);
    KDNode(const KDNode &node);
    KDNode();
    ~KDNode();
    std::string label();
};

#endif // KDNODE_H
