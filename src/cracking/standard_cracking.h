#ifndef MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
#define MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H

#include "avl_tree.h"
#include "stdio.h"
#include <cstdlib>
#include "../util/structs.h"
#include "../util/util.h"
#include "../util/define.h"
#include <array>
using namespace std;

void cracking_pre_processing(Table *table, Tree * t);
void cracking_partial_built(Table *table, Tree * T,vector<array<int64_t, 3>>  *rangequeries);
void cracking_index_lookup(Tree * T,vector<array<int64_t, 3>>  *rangequeries,vector<pair<int,int>>  *offsets);
void cracking_scan(Table *table, vector<array<int64_t, 3>> *query, vector<pair<int,int>>  *offsets, vector<int64_t> * result);
void cracking_intersection(Table *table,vector<pair<int,int>>  *offsets, vector<vector<bool> > *bitmaps, vector<int64_t> * result);
#endif //MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
