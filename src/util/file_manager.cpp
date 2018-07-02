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

vector<int64_t> split_int(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<int64_t> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(strtoll(item.c_str(), NULL, 10));
    }
    return tokens;
}

void loadQueries(RangeQuery *rangequeries, string QUERIES_FILE_PATH, int64_t NUM_QUERIES, int64_t NUMBER_OF_COLUMNS)
{
    ifstream infile(QUERIES_FILE_PATH.c_str());
    if (!infile)
    {
        fprintf(stderr, "Cannot open file. \n");
        return;
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        rangequeries[i].leftpredicate = (int64_t *)malloc(sizeof(int64_t) * NUM_QUERIES);
        rangequeries[i].rightpredicate = (int64_t *)malloc(sizeof(int64_t) * NUM_QUERIES);
    }

    string line;
    size_t line_index = 0;
    while (getline(infile, line))
    {
        vector<string> predicates = split_string(line, '|');       
        for(size_t col = 0; col < predicates.size(); col++)
        {
            vector<int64_t> elements = split_int(predicates[col], ';');
            rangequeries[col].leftpredicate[line_index] = elements[0];
            rangequeries[col].rightpredicate[line_index] = elements[2];
        }
        
        line_index++;
    }
}

void loadcolumn(Column *c, string COLUMN_FILE_PATH, int64_t COLUMN_SIZE, int64_t NUMBER_OF_COLUMNS)
{
    ifstream infile(COLUMN_FILE_PATH.c_str());
    if (!infile)
    {
        fprintf(stderr, "Cannot open file. \n");
        return;
    }

    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        c[i].data = (int64_t *)malloc(sizeof(int64_t) * COLUMN_SIZE);
    }

    string line;
    size_t line_index = 0;
    while (getline(infile, line))
    {
        vector<int64_t> elements = split_int(line, '|');
        
        for(size_t col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            c[col].data[line_index] = elements[col];
        }
        line_index++;
    }
}