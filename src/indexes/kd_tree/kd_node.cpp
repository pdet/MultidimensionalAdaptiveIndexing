#include "kd_node.hpp"
#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <stdexcept>

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
    label += "Position: " + to_string(position)+ "\n";
    return label; 
}

bool KDNode::node_greater_equal_query(Query& query){
    for(int64_t i = 0; i < query.predicate_count(); i++)
    {
        if(column == query.predicates.at(i).column){
            auto high = query.predicates.at(i).high;
            return high <= key;
        }
    }
    throw std::invalid_argument("Node column not in query");
}

bool KDNode::node_less_equal_query(Query& query){
    for(int64_t i = 0; i < query.predicate_count(); i++)
    {
        if(column == query.predicates.at(i).column){
            auto low = query.predicates.at(i).low;
            return key <= low;
        }
    }
    throw std::invalid_argument("Node column not in query");
}
