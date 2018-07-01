#ifndef MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
#define MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H

#include "avl_tree.h"
#include "stdio.h"
#include <cstdlib>
#include "../util/structs.h"
#include <boost/dynamic_bitset.hpp>
#include "../util/util.h"
using namespace std;

void cracking_pre_processing(Table *table, Tree * t);
void cracking_partial_built(Table *table, Tree * T,vector<pair<int64_t,int64_t>>  *rangequeries);
void cracking_index_lookup(Tree * T,vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>>  *offsets);
void cracking_intersection(Table *table,vector<pair<int,int>>  *offsets, vector<boost::dynamic_bitset<>> *bitmaps, int64_t * result);
#endif //MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
