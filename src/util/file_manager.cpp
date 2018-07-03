#include "file_manager.h"
#include "structs.h"
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sstream>

using namespace std;

vector<string> split_string(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void tpch_loadData(Table &t, string DATA_FILE_PATH){
    ifstream infile(DATA_FILE_PATH.c_str());
    if (!infile)
    {
        fprintf(stderr, "Cannot open file. \n");
        exit(-1);
    }
    string line;
    size_t line_index = 1;
    t.columns.resize(11);
    t.s_columns.resize(5);

    while (getline(infile, line))
    {
        vector<string> predicates = split_string(line, ';');
        // for (int i = 0; i < predicates.size(); ++i)
        //     std::cout << predicates[i] << '\n';
        t.ids.push_back(line_index);
        // int cols: 1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13
        t.columns.at(0).push_back(strtoll(predicates.at(0).c_str(), NULL, 10));
        t.columns.at(1).push_back(strtoll(predicates.at(1).c_str(), NULL, 10));
        t.columns.at(2).push_back(strtoll(predicates.at(2).c_str(), NULL, 10));
        t.columns.at(3).push_back(strtoll(predicates.at(3).c_str(), NULL, 10));
        t.columns.at(4).push_back(strtoll(predicates.at(4).c_str(), NULL, 10));
        t.columns.at(8).push_back(strtoll(predicates.at(10).c_str(), NULL, 10));
        t.columns.at(9).push_back(strtoll(predicates.at(11).c_str(), NULL, 10));
        t.columns.at(10).push_back(strtoll(predicates.at(12).c_str(), NULL, 10));
        // cols 6, 7, 8 are floats so we multiply by 100
        t.columns.at(5).push_back(atof(predicates.at(5).c_str()) * 100);
        t.columns.at(6).push_back(atof(predicates.at(6).c_str()) * 100);
        t.columns.at(7).push_back(atof(predicates.at(7).c_str()) * 100);

        // string cols: 9, 10, 14, 15, 16
        t.s_columns.at(0).push_back(predicates.at(8));
        t.s_columns.at(1).push_back(predicates.at(9));
        t.s_columns.at(2).push_back(predicates.at(13));
        t.s_columns.at(3).push_back(predicates.at(14));
        t.s_columns.at(4).push_back(predicates.at(15));
        line_index++;
    }
}

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