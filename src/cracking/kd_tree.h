#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include "cracker_index.h"
#include "stdio.h"

// #include "structs.h"
#include <cstdlib>
#include <stdlib.h>
using namespace std;
struct Table
{
    vector<int64_t> ids;
    vector<vector<int64_t>> columns;
};

struct KDNode;

typedef struct KDNode *KDTree;

int64_t SearchKDTree(KDTree &index, vector<pair<int64_t, int64_t>> query, Table &table, bool should_crack, size_t currentQueryNum);

vector<int64_t> SearchKDTreeProgressive(KDTree &tree, vector<pair<int64_t, int64_t>> query, Table &table, bool should_crack, size_t currentQueryNum);

KDTree FullKDTree(Table &table);

void freeKDTree(KDTree tree);

void configKDTree(int64_t threshold);

KDTree CreateNode(int64_t column, int64_t element, int64_t left_position, int64_t right_position);

pair<int64_t, int64_t> find_median(Table &table, int64_t column, int64_t lower_limit, int64_t upper_limit);

void Print( KDTree T );

int select_rq_scan_new (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);
int select_rq_scan_sel_vec (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);

// void PatialKDTree(vector<pair<KDTree, int64_t>> *nodes, int n_of_cols);

#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H