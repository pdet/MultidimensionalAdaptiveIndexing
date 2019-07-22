#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include "kd_node.hpp"

using namespace std;

KDNode::KDNode(size_t column, float key, size_t left_position, size_t right_position)
: key(key), column(column), left_position(left_position), right_position(right_position){
    left_child = nullptr;
    right_child = nullptr;
}
KDNode::~KDNode(){}