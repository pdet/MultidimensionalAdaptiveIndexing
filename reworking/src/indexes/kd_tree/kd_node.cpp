#ifndef KDNODE
#define KDNODE

#include "../helpers/helpers.cpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>

using namespace std;

class KDNode
{
public:

    float key; // Search-key
    size_t column; // Search column

    unique_ptr<KDNode> left_child;
    unique_ptr<KDNode> right_child;

    int64_t left_position; // Strict smaller than key from this position backwards
    int64_t right_position; // Greater or equal than key from this position forwards


    KDNode(size_t column, float key, int64_t left_position, int64_t right_position)
    : column(column), key(key), left_position(left_position), right_position(right_position){
        left_child = nullptr;
        right_child = nullptr;
    }
    ~KDNode(){}
};

#endif // KDNODE