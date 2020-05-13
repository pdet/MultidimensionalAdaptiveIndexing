#include "kd_node.hpp"
#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <assert.h>

using namespace std;

KDNode::KDNode(size_t column, float key, size_t position)
: key(key), column(column), left_child(nullptr), right_child(nullptr), position(position),
  current_start(position), current_end(position){}

KDNode::KDNode(const KDNode &node)
    : KDNode(node.column, node.key, node.position){}

KDNode::KDNode()
: key(0), column(0), left_child(nullptr), right_child(nullptr), position(0){}

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
    if(query.predicates[column].high < key)
        return +1;
    return 0;
}

bool KDNode::noChildren(){
  return !(left_child.get() || right_child.get());
}

void KDNode::setLeft(std::unique_ptr<KDNode> child){
    assert(!this->left_child);
    this->left_child = move(child);
}
void KDNode::setRight(std::unique_ptr<KDNode> child){
    assert(!this->right_child);
    this->right_child = move(child);
}
