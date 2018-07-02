#include "file_manager.h"
#include "structs.h"
#include <vector>

using namespace std;

void loadQueries(RangeQuery *rangequeries, string QUERIES_FILE_PATH, int64_t NUM_QUERIES, int64_t NUMBER_OF_COLUMNS)
{
    FILE *f = fopen(QUERIES_FILE_PATH.c_str(), "r");
    if (!f)
    {
        fprintf(stderr, "Cannot open file. \n");
        return;
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        rangequeries[i].leftpredicate = (int64_t *)malloc(sizeof(int64_t) * NUM_QUERIES);
        fread(rangequeries[i].leftpredicate, sizeof(int64_t), NUM_QUERIES, f);

        rangequeries[i].rightpredicate = (int64_t *)malloc(sizeof(int64_t) * NUM_QUERIES);
        fread(rangequeries[i].rightpredicate, sizeof(int64_t), NUM_QUERIES, f);
    }
    fclose(f);
}

void loadcolumn(Column *c, string COLUMN_FILE_PATH, int64_t COLUMN_SIZE, int64_t NUMBER_OF_COLUMNS)
{
    FILE *f = fopen(COLUMN_FILE_PATH.c_str(), "r");
    if (!f)
    {
        fprintf(stderr, "Cannot open file. \n");
        return;
    }

    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        c[i].data = (int64_t *)malloc(sizeof(int64_t) * COLUMN_SIZE);
        fread(c[i].data, sizeof(int64_t), COLUMN_SIZE, f);
    }
    fclose(f);
}