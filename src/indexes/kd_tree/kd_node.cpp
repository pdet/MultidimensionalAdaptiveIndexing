#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include "kd_node.hpp"
#include <string>

using namespace std;

KDNode::KDNode(int64_t column, float key, int64_t left_position, int64_t right_position)
: key(key), column(column), left_position(left_position), right_position(right_position){
    left_child = nullptr;
    right_child = nullptr;
}

KDNode::KDNode(const KDNode &node)
    : KDNode(node.column, node.key, node.left_position, node.right_position){}

KDNode::KDNode(){
    key = 0;
    column = 0;
    left_position = 0;
    right_position = 0;
    left_child = nullptr;
    right_child = nullptr;
}

KDNode::~KDNode(){}

string KDNode::label(){
    string label;
    label += "Key: " + to_string(key) + "\n";
    label += "Column: " + to_string(column) + "\n";
    label += to_string(left_position) + "  |  " + to_string(right_position)+ "\n";
    return label; 
}
