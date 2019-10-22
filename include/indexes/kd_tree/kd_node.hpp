#ifndef KDNODE_H
#define KDNODE_H

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <memory>

using namespace std;

class KDNode;

class KDNode
{
public:

    float key; // Search-key
    int64_t column; // Search column

    unique_ptr<KDNode> left_child;
    unique_ptr<KDNode> right_child;

    int64_t left_position; // Strict smaller than key from this position backwards
    int64_t right_position; // Greater or equal than key from this position forwards


    KDNode(int64_t column, float key, int64_t left_position, int64_t right_position);
    KDNode(const KDNode &node);
    KDNode();
    ~KDNode();
};

#endif // KDNODE_H
