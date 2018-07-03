#ifndef LIGHTWEIGHTADAPTIVEINDEXING_FILEMANAGER_H
#define LIGHTWEIGHTADAPTIVEINDEXING_FILEMANAGER_H
#include <vector>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "structs.h"

using namespace std;

void loadQueries(RangeQuery *rangequeries,string QUERIES_FILE_PATH, int64_t NUM_QUERIES, int64_t NUMBER_OF_COLUMNSs);
void loadcolumn(Column *c,string COLUMN_FILE_PATH, int64_t COLUMN_SIZE, int64_t NUMBER_OF_COLUMNS);
void tpch_loadData(Table &t, string DATA_FILE_PATH);

#endif //LIGHTWEIGHTADAPTIVEINDEXING_FILEMANAGER_H
