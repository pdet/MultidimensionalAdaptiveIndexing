#ifndef MULTIDEMIONSIONALINDEXING_UTIL_H
#define MULTIDEMIONSIONALINDEXING_UTIL_H

#include "structs.h"
#include "define.h"
#include <array>

using namespace std;


void create_bitmap(IndexEntry *c, int64_t from, int64_t to, vector<bool> &bitmap);
vector<int64_t> join_bitmaps(vector<vector<bool> > *bitmaps);
int select_rq_scan_new (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);
int select_rq_scan_sel_vec (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);
void full_scan(Table *table, vector<array<int64_t, 3>>  *rangequeries,vector<pair<int,int>> *offsets, vector<int64_t> * result);

void print_help(int argc, char** argv);
pair<string,string> split_once(string delimited, char delimiter);
#endif //MULTIDEMIONSIONALINDEXING_UTIL_H
