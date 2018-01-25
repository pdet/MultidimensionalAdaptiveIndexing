//
// Created by Pedro Holanda on 25/01/2018.
//

#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "file_manager.h"
#include "kd_tree.h"
#include "../util/structs.h"

//#define VERIFY

std::string COLUMN_FILE_PATH, QUERIES_FILE_PATH;
int64_t COLUMN_SIZE;
int64_t NUM_QUERIES, NUMBER_OF_COLUMNS;

std::set<int64_t> range_query_baseline(Column *c, RangeQuery *queries, size_t query_index)
{
    std::set<int64_t> ids;
    for (size_t i = 0; i <= COLUMN_SIZE - 1; ++i)
    {
        bool is_valid = true;
        for (size_t j = 0; j < NUMBER_OF_COLUMNS && is_valid; ++j)
        {
            int64_t keyL = queries[j].leftpredicate[query_index];
            int64_t keyH = queries[j].rightpredicate[query_index];
            if (!(c[j].data[i] >= keyL && c[j].data[i] < keyH))
            {
                is_valid = false;
            }
        }
        if (is_valid)
        {
            ids.insert(i);
        }
    }
    return ids;
}

bool verify_range_query(Column *c, RangeQuery *queries, size_t query_index, std::set<int64_t> received)
{
    std::set<int64_t> r1 = range_query_baseline(c, queries, query_index);
    if (received != r1)
    {
        fprintf(stderr, "Incorrect Results!\n");
        fprintf(stderr, "Expected:\n");
        std::set<int64_t>::iterator it;
        for (it = r1.begin(); it != r1.end(); ++it)
        {
            fprintf(stderr, "%ld ", *it);
        }
        fprintf(stderr, "\n");

        fprintf(stderr, "Got:\n");
        for (it = received.begin(); it != received.end(); ++it)
        {
            fprintf(stderr, "%ld ", *it);
        }
        fprintf(stderr, "\n");
        assert(0);
        return false;
    }
    return true;
}

void progressive_kd_tree(std::vector<double> *response_times)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    start = std::chrono::system_clock::now();
    std::vector<Row> crackerrows(COLUMN_SIZE);
    for (size_t line = 0; line < COLUMN_SIZE; ++line)
    {
        crackerrows.at(line).id = line;
        crackerrows.at(line).data = std::vector<int64_t>(NUMBER_OF_COLUMNS);
        for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col)
        {
            crackerrows.at(line).data.at(col) = c[col].data[line];
        }
    }
    std::vector<std::pair<KDTree, int64_t>> nodes;
    KDTree index = CreateNode(0, find_median(crackerrows, 0), crackerrows);

    nodes.push_back(std::make_pair(index, 0));

    PatialKDTree(&nodes, NUMBER_OF_COLUMNS);
    end = std::chrono::system_clock::now();
    response_times->at(0) += std::chrono::duration<double>(end - start).count();

    for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index)
    {
        PatialKDTree(&nodes, NUMBER_OF_COLUMNS);
        // Transform query in a format easier to handle
        std::vector<std::pair<int64_t, int64_t>> query(NUMBER_OF_COLUMNS);
        for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
        {
            query.at(i).first = rangequeries[i].leftpredicate[query_index];
            query.at(i).second = rangequeries[i].rightpredicate[query_index];
        }

        start = std::chrono::system_clock::now();
        std::vector<int64_t> result = SearchKDTree(index, query, crackerrows, false);
        end = std::chrono::system_clock::now();
        response_times->at(query_index) += std::chrono::duration<double>(end - start).count();

#ifdef VERIFY
        std::set<int64_t> final_ids;
        for (size_t i = 0; i < result.size(); ++i)
        {
            int64_t id = result.at(i);
            final_ids.insert(id);
        }
        bool pass = verify_range_query(c, rangequeries, query_index, final_ids);
        if (pass == 0)
            std::cout << "Query : " << query_index << " " << pass << "\n";
#endif
    }

    freeKDTree(index);
    for(int i = 0; i < NUMBER_OF_COLUMNS; ++i){
        free(c[i].data);
        free(rangequeries[i].leftpredicate);
        free(rangequeries[i].rightpredicate);
    }

    free(c);
    free(rangequeries);
}
//.column.txt .query.txt 10 1000 0 2
int main(int argc, char **argv)
{
    int INDEXING_TYPE;

    if (argc < 6)
    {
        printf("Missing mandatory parameters\n");
        return -1;
    }

    COLUMN_FILE_PATH = argv[1];
    QUERIES_FILE_PATH = argv[2];
    NUM_QUERIES = std::stoi(argv[3]);
    COLUMN_SIZE = atoi(argv[4]);
    INDEXING_TYPE = atoi(argv[5]);
    NUMBER_OF_COLUMNS = atoi(argv[6]);

    //Progressive KD-Tree
    if (INDEXING_TYPE == 0)
    {
        std::vector<double> progressivekdtree(NUM_QUERIES);
        progressive_kd_tree(&progressivekdtree);
        for (int q = 0; q < NUM_QUERIES; q++)
            std::cout << progressivekdtree[q] << "\n";
    }


}
