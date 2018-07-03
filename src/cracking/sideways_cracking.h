#ifndef MULTIDEMIONSIONALINDEXING_SIDEWAYSCRACKING_H
#define MULTIDEMIONSIONALINDEXING_SIDEWAYSCRACKING_H

#include "avl_tree.h"
#include "stdio.h"
#include <cstdlib>
#include "../util/structs.h"
#include "../util/util.h"
#include "../util/define.h"
using namespace std;

void sideways_cracking_pre_processing(Table *table, Tree * T);
void sideways_cracking_partial_built(Table *table, Tree * T,vector<pair<int64_t,int64_t>>  *rangequeries);
void sideways_cracking_index_lookup(Tree * T,vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>>  *offsets);
void sideways_cracking_scan(Table *table, vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>> *offsets, vector<int64_t> * result);
#endif //MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H

