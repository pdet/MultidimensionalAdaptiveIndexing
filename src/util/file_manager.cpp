//
// Created by PHolanda on 17/12/17.
//

#include <vector>
#include "structs.h"
#include "file_manager.h"

void loadQueries(RangeQuery *rangequeries,std::string QUERIES_FILE_PATH, int64_t NUM_QUERIES){
    FILE *f = fopen(QUERIES_FILE_PATH.c_str(), "r");
    if (!f) {
        printf("Cannot open file.\n");
        return;
    }
    int64_t *temp_data = (int64_t *) malloc(sizeof(int64_t) * NUM_QUERIES);
    fread(temp_data, sizeof(int64_t), NUM_QUERIES, f);
    rangequeries->leftpredicate = std::vector<int64_t>(NUM_QUERIES);
    for (size_t i = 0; i < NUM_QUERIES; i++) {
        rangequeries->leftpredicate[i] = temp_data[i];
    }
    fread(temp_data, sizeof(int64_t), NUM_QUERIES, f);
    rangequeries->rightpredicate = std::vector<int64_t>(NUM_QUERIES);
    for (size_t i = 0; i < NUM_QUERIES; i++) {
        rangequeries->rightpredicate[i] = temp_data[i];
    }
    fclose(f);

}

void loadcolumn(Column *c,std::string COLUMN_FILE_PATH, int64_t COLUMN_SIZE){
    FILE *f = fopen(COLUMN_FILE_PATH.c_str(), "r");
    if (!f) {
        printf("Cannot open file.\n");
        return;
    }
    int64_t *temp_data = (int64_t *) malloc(sizeof(int64_t) * COLUMN_SIZE);
    fread(temp_data, sizeof(int64_t), COLUMN_SIZE, f);
    c->data = std::vector<int64_t>(COLUMN_SIZE);
    for (size_t i = 0; i < COLUMN_SIZE; i++) {

        c->data[i] = temp_data[i];
    }
    fclose(f);
}