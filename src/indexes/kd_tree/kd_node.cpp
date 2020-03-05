#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include "kd_node.hpp"
#include <string>

using namespace std;

KDNode::KDNode(int64_t column, float key, int64_t position)
: key(key), column(column), position(position){
    left_child = nullptr;
    right_child = nullptr;
}

KDNode::KDNode(const KDNode &node)
    : KDNode(node.column, node.key, node.position){}

KDNode::KDNode(){
    key = 0;
    column = 0;
    position = 0;
    left_child = nullptr;
    right_child = nullptr;
}

KDNode::~KDNode(){}

string KDNode::label(){
    string label;
    label += "Key: " + to_string(key) + "\n";
    label += "Column: " + to_string(column) + "\n";
    label += to_string(position)+ "\n";
    return label; 
}

int8_t KDNode::compare(Query& query){
    if(key <= query.predicates[column].low) 
        return -1;
    if(query.predicates[column].high <= key)
        return +1;
    return 0;
}
