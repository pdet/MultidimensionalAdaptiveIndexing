#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include "stdio.h"
#include "../util/util.h"
#include <cstdlib>
#include <stdlib.h>
#include <array>
using namespace std;

Tree FullTree(Table &table);

void freeTree(Tree tree);

void Print( Tree T );

void full_kdtree_pre_processing(Table *table, Tree * T);
void cracking_kdtree_pre_processing(Table *table, Tree * T);
void cracking_kdtree_partial_built(Table *table, Tree * T, vector<array<int64_t, 3>> *rangequeries);
void kdtree_index_lookup(Tree * tree, vector<array<int64_t, 3>> *query,vector<pair<int,int>>  *offsets);
void kdtree_scan(Table *table, vector<array<int64_t, 3>> *query, vector<pair<int,int>>  *offsets, vector<int64_t> * result);
#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H