#include "kd_tree.h"

struct Row {
    int64_t id;
    int64_t *data;
};

struct KDNode{
    ElementType element;
    int64_t column;

    KDNode* left;
    KDNode* right;

    Row* rows;
};

KDTree InitializeKDTree(int64_t number_of_rows, int64_t number_of_columns, IndexEntry **columns){
    KDTree tree = (KDTree) malloc(sizeof(struct KDNode));

    tree->left = NULL;
    tree->right = NULL;
    tree->element = 0;
    tree->column = 0;
    tree->rows = (Row*) malloc(number_of_rows * sizeof(Row));

    for (size_t i = 0; i < number_of_rows; ++i) {
        tree->rows[i].id = i;
        tree->rows[i].data = (int64_t*) malloc(number_of_columns * sizeof(int64_t));
        for(size_t j = 0; j < number_of_columns; ++j){
            tree->rows[i].data[j] = columns[j][i].m_key;
        }
    }

    return tree;
}