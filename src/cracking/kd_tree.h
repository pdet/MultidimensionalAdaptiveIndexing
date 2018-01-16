#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include <cstdlib>
#include <stdlib.h>
#include "cracker_index.h"

typedef int64_t ElementType;

struct Row {
    int64_t id;
    std::vector<int64_t> data;
};

struct KDNode;

typedef struct KDNode* KDTree;

KDTree InitializeKDTree(int64_t number_of_rows, int64_t number_of_columns, IndexEntry **table);

std::vector<int64_t > SearchKDTree(KDTree index, std::vector<std::pair<int64_t, int64_t>> query, std::vector<Row> lines);

KDTree Insert(KDTree tree, int64_t column, ElementType element, std::vector<Row> &lines);

#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H