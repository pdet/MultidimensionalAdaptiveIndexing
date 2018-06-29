//
// Created by PHolanda on 17/12/17.
//

#ifndef MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
#define MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H

#include "avl_tree.h"
#include "cracker_index.h"
#include "stdio.h"
#include <cstdlib>
#include "../util/structs.h"
#include <boost/dynamic_bitset.hpp>
#include "../util/util.h"
using namespace std;

void cracking_pre_processing(Column *c,IndexEntry **crackercolumns, AvlTree * t);
void cracking_partial_built(IndexEntry **crackercolumns, AvlTree * T,vector<pair<int64_t,int64_t>>  *rangequeries);
void cracking_index_lookup(AvlTree * T,vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>>  *offsets);
void cracking_intersection(IndexEntry **crackercolumns,vector<pair<int,int>>  *offsets, vector<boost::dynamic_bitset<>> *bitmaps, int64_t * result);
#endif //MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
