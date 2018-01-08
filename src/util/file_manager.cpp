//
// Created by PHolanda on 17/12/17.
//

#include <vector>
#include "structs.h"
#include "file_manager.h"

void loadQueries(RangeQuery *rangequeries, std::string QUERIES_FILE_PATH, int64_t NUM_QUERIES, int64_t NUMBER_OF_COLUMNS){
    FILE *f = fopen(QUERIES_FILE_PATH.c_str(), "r");
    if (!f) {
        printf("Cannot open file.\n");
        return;
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i) {
        int64_t *temp_data = (int64_t *) malloc(sizeof(int64_t) * NUM_QUERIES);
        fread(temp_data, sizeof(int64_t), NUM_QUERIES, f);
        rangequeries[i].leftpredicate = std::vector<int64_t>(NUM_QUERIES);
        for (size_t j = 0; j < NUM_QUERIES; j++) {
            rangequeries[i].leftpredicate[j] = temp_data[j];
        }
        fread(temp_data, sizeof(int64_t), NUM_QUERIES, f);
        rangequeries[i].rightpredicate = std::vector<int64_t>(NUM_QUERIES);
        for (size_t j = 0; j < NUM_QUERIES; j++) {
            rangequeries[i].rightpredicate[j] = temp_data[j];
        }
    }
    fclose(f);

}

void loadcolumn(Column *c,std::string COLUMN_FILE_PATH, int64_t COLUMN_SIZE, int64_t NUMBER_OF_COLUMNS){
    FILE *f = fopen(COLUMN_FILE_PATH.c_str(), "r");
    if (!f) {
        printf("Cannot open file.\n");
        return;
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i) {
        int64_t *temp_data = (int64_t *) malloc(sizeof(int64_t) * COLUMN_SIZE);
        fread(temp_data, sizeof(int64_t), COLUMN_SIZE, f);
        c[i].data = std::vector<int64_t>(COLUMN_SIZE);
        for (size_t j = 0; j < COLUMN_SIZE; j++) {

            c[i].data[j] = temp_data[j];
        }
    }
    fclose(f);
}