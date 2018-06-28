#ifndef MULTIDEMIONSIONALINDEXING_UTIL_H
#define MULTIDEMIONSIONALINDEXING_UTIL_H

#include <boost/dynamic_bitset.hpp>
#include "structs.h"
#include "../cracking/cracker_index.h"

using namespace std;


void scanQuery(IndexEntry *c, int64_t from, int64_t to, boost::dynamic_bitset<> &bitmap);
int64_t sum_bitmap(boost::dynamic_bitset<> bitmap, Column first_column);
int64_t join_bitmaps(std::vector<boost::dynamic_bitset<>> bitmaps, Column * columns);
int select_rq_scan_new (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);
int select_rq_scan_sel_vec (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n);
int full_scan(Column *c, vector<pair<int64_t,int64_t>>  *rangequeries);

#endif //MULTIDEMIONSIONALINDEXING_UTIL_H
