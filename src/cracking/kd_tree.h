#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include <cstdlib>
#include <stdlib.h>
#include "cracker_index.h"

typedef int64_t ElementType;

struct Row;

struct KDNode;

typedef struct KDNode* KDTree;

KDTree InitializeKDTree(int64_t number_of_rows, int64_t number_of_columns, IndexEntry **table);

#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H