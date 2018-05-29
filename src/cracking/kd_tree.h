#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include "cracker_index.h"
#include "stdio.h"

// #include "structs.h"
#include <cstdlib>
#include <stdlib.h>

typedef int64_t ElementType;

struct Table
{
    std::vector<int64_t> ids;
    std::vector<std::vector<ElementType>> columns;
};

struct KDNode;

typedef struct KDNode *KDTree;

std::vector<int64_t> SearchKDTree(KDTree &index, std::vector<std::pair<int64_t, int64_t>> query, Table &table, bool should_crack, size_t currentQueryNum);

std::vector<int64_t> SearchKDTreeProgressive(KDTree &tree, std::vector<std::pair<int64_t, int64_t>> query, Table &table, bool should_crack, size_t currentQueryNum);

KDTree FullKDTree(Table &table);

void freeKDTree(KDTree tree);

void configKDTree(int64_t threshold);

KDTree CreateNode(int64_t column, ElementType element, int64_t left_position, int64_t right_position);

std::pair<int64_t, int64_t> find_median(Table &table, int64_t column, int64_t lower_limit, int64_t upper_limit);

void Print( KDTree T );
// void PatialKDTree(std::vector<std::pair<KDTree, int64_t>> *nodes, int n_of_cols);

#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H