#ifndef MULTIDEMIONSIONALINDEXING_UTIL_H
#define MULTIDEMIONSIONALINDEXING_UTIL_H

#include <boost/dynamic_bitset.hpp>
#include "structs.h"

using namespace std;


void create_bitmap(IndexEntry *c, int64_t from, int64_t to, boost::dynamic_bitset<> &bitmap);
int64_t join_bitmaps(vector<boost::dynamic_bitset<>> *bitmaps);
int select_rq_scan_new (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);
int select_rq_scan_sel_vec (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);
void full_scan(Table *table, vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>> *offsets, int64_t * result);

void print_help(int argc, char** argv);
pair<string,string> split_once(string delimited, char delimiter);
#endif //MULTIDEMIONSIONALINDEXING_UTIL_H
