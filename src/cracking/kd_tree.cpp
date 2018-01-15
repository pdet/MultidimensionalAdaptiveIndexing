#include "kd_tree.h"

//struct Row {
//    int64_t id;
//    int64_t *data;
//};

struct KDNode{
    ElementType element;
    int64_t column;

    KDNode* left;
    KDNode* right;

//    Row* rows;
};