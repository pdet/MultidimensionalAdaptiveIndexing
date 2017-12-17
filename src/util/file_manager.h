//
// Created by PHolanda on 17/12/17.
//

#ifndef LIGHTWEIGHTADAPTIVEINDEXING_FILEMANAGER_H
#define LIGHTWEIGHTADAPTIVEINDEXING_FILEMANAGER_H
#include "../cracking/cracker_index.h"
#include "structs.h"


void loadQueries(RangeQuery *rangequeries,std::string QUERIES_FILE_PATH, int64_t NUM_QUERIES);
void loadcolumn(Column *c,std::string COLUMN_FILE_PATH, int64_t COLUMN_SIZE);


#endif //LIGHTWEIGHTADAPTIVEINDEXING_FILEMANAGER_H
