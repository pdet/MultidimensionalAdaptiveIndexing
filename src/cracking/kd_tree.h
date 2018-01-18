#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include "cracker_index.h"
#include <cstdlib>
#include <stdlib.h>

typedef int64_t ElementType;

struct Row
{
    int64_t id;
    std::vector<int64_t> data;
};

struct KDNode;

typedef struct KDNode *KDTree;

std::vector<int64_t> SearchKDTree(KDTree &index, std::vector<std::pair<int64_t, int64_t>> query, std::vector<Row> lines, bool should_crack);

KDTree FullKDTree(std::vector<Row> lines);

void freeKDTree(KDTree tree);

#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H