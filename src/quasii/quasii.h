#ifndef QUASII
#define QUASII
#include "stdio.h"
#include <cstdlib>
#include "../util/structs.h"
#include "../util/util.h"
#include "../util/define.h"
#include <array>
#include <cmath>
#include <stack>
#include <algorithm>


void quasii_pre_processing(Table *table, void*);
void quasii_partial_built(Table *table, void*, vector<array<int64_t, 3>>  *rangequeries);

void quasii_index_lookup(void*, vector<array<int64_t, 3>>  *rangequeries, vector<pair<int,int>>  *offsets);

void quasii_scan(Table *table, vector<array<int64_t, 3>> *query, vector<pair<int,int>>  *offsets, vector<int64_t> * result);
#endif //QUASII