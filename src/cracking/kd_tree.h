#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include "cracker_index.h"
#include <cstdlib>
#include <stdlib.h>
#include "structs.h"

typedef int64_t ElementType;



struct KDNode;

typedef struct KDNode *KDTree;

std::vector<int64_t> SearchKDTree(KDTree &index, std::vector<std::pair<int64_t, int64_t>> query, std::vector<Row> lines, bool should_crack);

KDTree FullKDTree(std::vector<Row> lines);

void freeKDTree(KDTree tree);

void PatialKDTree(std::vector<std::pair<KDTree, int64_t>> *nodes, int n_of_cols);

KDTree CreateNode(int64_t column, ElementType element, std::vector<Row> lines);
int64_t find_median(std::vector<Row> lines, int64_t column);


#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H