//
// Created by PHolanda on 17/12/17.
//

#include "file_manager.h"
#include "structs.h"
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sstream>

std::vector<std::string> split_string(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

std::vector<int64_t> split_int(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<int64_t> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(strtoll(item.c_str(), NULL, 10));
    }
    return tokens;
}

void loadQueries(RangeQuery *rangequeries, std::string QUERIES_FILE_PATH, int64_t NUM_QUERIES, int64_t NUMBER_OF_COLUMNS)
{
    std::ifstream infile(QUERIES_FILE_PATH.c_str());
    if (!infile)
    {
        printf("Cannot open file.\n");
        return;
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        rangequeries[i].leftpredicate = (int64_t *)malloc(sizeof(int64_t) * NUM_QUERIES);
        rangequeries[i].rightpredicate = (int64_t *)malloc(sizeof(int64_t) * NUM_QUERIES);
    }

    std::string line;
    size_t line_index = 0;
    while (std::getline(infile, line))
    {
        std::vector<std::string> predicates = split_string(line, '|');       
        for(size_t col = 0; col < predicates.size(); col++)
        {
            std::vector<int64_t> elements = split_int(predicates[col], ';');
            rangequeries[col].leftpredicate[line_index] = elements[0];
            rangequeries[col].rightpredicate[line_index] = elements[2];
        }
        
        line_index++;
    }
}

void loadcolumn(Column *c, std::string COLUMN_FILE_PATH, int64_t COLUMN_SIZE, int64_t NUMBER_OF_COLUMNS)
{
    std::ifstream infile(COLUMN_FILE_PATH.c_str());
    if (!infile)
    {
        printf("Cannot open file.\n");
        return;
    }

    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        c[i].data = (int64_t *)malloc(sizeof(int64_t) * COLUMN_SIZE);
    }

    std::string line;
    size_t line_index = 0;
    while (std::getline(infile, line))
    {
        std::vector<int64_t> elements = split_int(line, '|');
        
        for(size_t col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            c[col].data[line_index] = elements[col];
        }
        line_index++;
    }
}