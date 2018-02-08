//
// Created by PHolanda on 17/12/17.
//
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "cracking/avl_tree.h"
#include "cracking/kd_tree.h"
#include "cracking/standard_cracking.h"
#include "fullindex/bulkloading_bp_tree.h"
#include "fullindex/hybrid_radix_insert_sort.h"
#include "util/file_manager.h"
#include "util/structs.h"

#define VERIFY

std::string COLUMN_FILE_PATH, QUERIES_FILE_PATH;
extern int64_t COLUMN_SIZE, BPTREE_ELEMENTSPERNODE;
int64_t NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD;

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
        fprintf(stderr, "Correct size: %d\n", int(r1.size()));
        fprintf(stderr, "Received size: %d\n", int(received.size()));
        fprintf(stderr, "Size diff: %d\n", int(r1.size()) - int(received.size()));
        fprintf(stderr, "--------------------\n");
        assert(0);
        return false;
    }
    return true;
}

void scanQuery(IndexEntry *c, int64_t from, int64_t to, std::vector<IndexEntry> &results)
{
    results.resize(0);
    for (size_t i = from; i <= to; i++)
    {
        results.push_back(IndexEntry(c[i].m_key, c[i].m_rowId));
    }
}

std::unordered_map<int64_t, bool> join_results(std::vector<std::vector<IndexEntry>> &partials)
{
    std::unordered_map<int64_t, bool> intersection;
    // Copy the first partial IDs
    for (size_t j = 0; j < partials[0].size(); ++j)
    {
        intersection.insert(std::make_pair(partials[0][j].m_rowId, true));
    }

    for (size_t i = 1; i < partials.size(); ++i)
    {
        std::unordered_map<int64_t, bool> tmp_intersection;
        for (size_t j = 0; j < partials[i].size(); ++j)
        {
            int64_t id = partials[i][j].m_rowId;
            // Check if id is inside intersection
            if (intersection.find(id) != intersection.end())
            {
                tmp_intersection.insert(std::make_pair(id, true));
            }
        }
        intersection = tmp_intersection;
    }

    return intersection;
}

std::set<int64_t> transform_result_to_set(std::unordered_map<int64_t, bool> result, std::vector<std::vector<IndexEntry>> partials)
{
    // Find the resulting rows and sum them
    std::set<int64_t> ids;
    std::unordered_map<int64_t, bool>::iterator it;
    for (it = result.begin(); it != result.end(); it++)
    {
        int64_t id = it->first;
        ids.insert(id);
    }
    return ids;
}

void standardCracking(std::vector<double> *standardcrackingtime)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;

    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    start = std::chrono::system_clock::now();
    IndexEntry **crackercolumns = (IndexEntry **)malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
    {
        crackercolumns[j] = (IndexEntry *)malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i)
        {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }
    //Initialitizing multiple Cracker Indexes
    AvlTree *T = (AvlTree *)malloc(sizeof(AvlTree) * NUMBER_OF_COLUMNS);
    for (size_t k = 0; k < NUMBER_OF_COLUMNS; ++k)
    {
        T[k] = NULL;
    }
    end = std::chrono::system_clock::now();
    standardcrackingtime->at(0) += std::chrono::duration<double>(end - start).count();

    std::vector<std::vector<IndexEntry>> partial_results(NUMBER_OF_COLUMNS);
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        partial_results.at(i).reserve(COLUMN_SIZE);
    }
    for (size_t i = 0; i < NUM_QUERIES; i++)
    {
        start = std::chrono::system_clock::now();
        for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
        {
            //Partitioning Column and Inserting in Cracker Indexing

            T[j] = standardCracking(
                crackercolumns[j],
                COLUMN_SIZE,
                T[j],
                rangequeries[j].leftpredicate[i],
                rangequeries[j].rightpredicate[i]);

            //Querying

            IntPair p1 = FindNeighborsGTE(rangequeries[j].leftpredicate[i], T[j], COLUMN_SIZE - 1);
            IntPair p2 = FindNeighborsLT(rangequeries[j].rightpredicate[i], T[j], COLUMN_SIZE - 1);
            int offset1 = p1->first;
            int offset2 = p2->second;
            free(p1);
            free(p2);

            scanQuery(crackercolumns[j], offset1, offset2, partial_results[j]);
        }
        std::unordered_map<int64_t, bool> result;
        // Join the partial results
        if (NUMBER_OF_COLUMNS == 1)
        {
            end = std::chrono::system_clock::now();
#ifdef VERIFY
            bool pass = verify_range_query(c, rangequeries, i, transform_result_to_set(join_results(partial_results), partial_results));
            if (pass == 0)
                std::cout << "Query : " << i << " " << pass << "\n";
#endif
        }
        else
        {
            result = join_results(partial_results);
            end = std::chrono::system_clock::now();
#ifdef VERIFY
            bool pass = verify_range_query(c, rangequeries, i, transform_result_to_set(join_results(partial_results), partial_results));
            if (pass == 0)
                std::cout << "Query : " << i << " " << pass << "\n";
#endif
        }
        standardcrackingtime->at(i) += std::chrono::duration<double>(end - start).count();
    }
    for (size_t l = 0; l < NUMBER_OF_COLUMNS; ++l)
    {
        free(crackercolumns[l]);
    }
    free(crackercolumns);
}

void filterQuery3(IndexEntry *c, RangeQuery queries, size_t query_index, int64_t from, int64_t to, std::vector<IndexEntry> &results)
{
    results.resize(0);
    int64_t keyL = queries.leftpredicate[query_index];
    int64_t keyH = queries.rightpredicate[query_index];
    for (size_t i = from; i <= to; ++i)
    {
        if (c[i].m_key >= keyL && c[i].m_key < keyH)
        {
            results.push_back(IndexEntry(c[i].m_key, c[i].m_rowId));
        }
    }
}

void full_scan(std::vector<double> *fullscantime)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;

    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    IndexEntry **crackercolumns = (IndexEntry **)malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
    {
        crackercolumns[j] = (IndexEntry *)malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i)
        {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }
    std::vector<std::vector<IndexEntry>> partial_results(NUMBER_OF_COLUMNS);
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        partial_results.at(i).reserve(COLUMN_SIZE);
    }
    for (size_t i = 0; i < NUM_QUERIES; i++)
    {
        start = std::chrono::system_clock::now();
        for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
        {
            filterQuery3(crackercolumns[j], rangequeries[j], i, 0, COLUMN_SIZE - 1, partial_results[j]);
        }
        std::unordered_map<int64_t, bool> result;
        // Join the partial results
        if (NUMBER_OF_COLUMNS == 1)
        {
            end = std::chrono::system_clock::now();
#ifdef VERIFY
            bool pass = verify_range_query(c, rangequeries, i, transform_result_to_set(join_results(partial_results), partial_results));
            if (pass == 0)
                std::cout << "Query : " << i << " " << pass << "\n";
#endif
        }
        else
        {
            result = join_results(partial_results);
            end = std::chrono::system_clock::now();
#ifdef VERIFY
            bool pass = verify_range_query(c, rangequeries, i, transform_result_to_set(join_results(partial_results), partial_results));
            if (pass == 0)
                std::cout << "Query : " << i << " " << pass << "\n";
#endif
        }
        fullscantime->at(i) += std::chrono::duration<double>(end - start).count();
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        free(crackercolumns[i]);
    }
    free(crackercolumns);
}

void *fullIndex(IndexEntry *c)
{
    hybrid_radixsort_insert(c, COLUMN_SIZE);
    void *I = build_bptree_bulk(c, COLUMN_SIZE);

    return I;
}

void bptree_bulk_index3(std::vector<double> *fullindex)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    start = std::chrono::system_clock::now();
    IndexEntry **crackercolumns = (IndexEntry **)malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
    {
        crackercolumns[j] = (IndexEntry *)malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i)
        {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }

    BulkBPTree **T = (BulkBPTree **)malloc(sizeof(BulkBPTree *) * NUMBER_OF_COLUMNS);
    for (size_t k = 0; k < NUMBER_OF_COLUMNS; ++k)
    {
        T[k] = (BulkBPTree *)fullIndex(crackercolumns[k]);
    }
    end = std::chrono::system_clock::now();
    fullindex->at(0) += std::chrono::duration<double>(end - start).count();

    std::vector<std::vector<IndexEntry>> partial_results(NUMBER_OF_COLUMNS);
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        partial_results.at(i).reserve(COLUMN_SIZE);
    }
    for (size_t i = 0; i < NUM_QUERIES; i++)
    {
        // query
        start = std::chrono::system_clock::now();
        for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
        {
            int64_t offset1 = (T[j])->gte(rangequeries[j].leftpredicate[i]);
            int64_t offset2 = (T[j])->lt(rangequeries[j].rightpredicate[i]);
            scanQuery(crackercolumns[j], offset1, offset2, partial_results[j]);
        }

        std::unordered_map<int64_t, bool> result;
        // Join the partial results
        if (NUMBER_OF_COLUMNS == 1)
        {
            end = std::chrono::system_clock::now();
#ifdef VERIFY
            bool pass = verify_range_query(c, rangequeries, i, transform_result_to_set(join_results(partial_results), partial_results));
            if (pass == 0)
                std::cout << "Query : " << i << " " << pass << "\n";
#endif
        }
        else
        {
            result = join_results(partial_results);
            end = std::chrono::system_clock::now();
#ifdef VERIFY
            bool pass = verify_range_query(c, rangequeries, i, transform_result_to_set(join_results(partial_results), partial_results));
            if (pass == 0)
                std::cout << "Query : " << i << " " << pass << "\n";
#endif
        }
        fullindex->at(i) += std::chrono::duration<double>(end - start).count();
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        free(crackercolumns[i]);
        free(T[i]);
    }
    free(crackercolumns);
    free(T);
}

void kdtree_cracking(std::vector<double> *response_times)
{
    configKDTree(KDTREE_THRESHOLD);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    start = std::chrono::system_clock::now();
    Table table;
    table.columns = std::vector<std::vector<ElementType>>(NUMBER_OF_COLUMNS);
    table.ids = std::vector<int64_t>(COLUMN_SIZE);
    for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col)
    {
        table.columns.at(col) = std::vector<ElementType>(COLUMN_SIZE);
        for (size_t line = 0; line < COLUMN_SIZE; ++line)
        {
            table.ids.at(line) = line;
            table.columns.at(col).at(line) = c[col].data[line];
        }
    }

    KDTree index = NULL;
    end = std::chrono::system_clock::now();

    response_times->at(0) += std::chrono::duration<double>(end - start).count();
    for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index)
    {
        // Transform query in a format easier to handle
        std::vector<std::pair<int64_t, int64_t>> query(NUMBER_OF_COLUMNS);
        for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
        {
            query.at(i).first = rangequeries[i].leftpredicate[query_index];
            query.at(i).second = rangequeries[i].rightpredicate[query_index];
        }

        start = std::chrono::system_clock::now();
        std::vector<int64_t> result = SearchKDTree(index, query, table, true);
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
    for (int i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        free(c[i].data);
        free(rangequeries[i].leftpredicate);
        free(rangequeries[i].rightpredicate);
    }

    free(c);
    free(rangequeries);
}

void full_kdtree_cracking(std::vector<double> *response_times)
{
    configKDTree(KDTREE_THRESHOLD);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    start = std::chrono::system_clock::now();
    Table table;
    table.columns = std::vector<std::vector<ElementType>>(NUMBER_OF_COLUMNS);
    table.ids = std::vector<int64_t>(COLUMN_SIZE);
    for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col)
    {
        table.columns.at(col) = std::vector<ElementType>(COLUMN_SIZE);
        for (size_t line = 0; line < COLUMN_SIZE; ++line)
        {
            table.ids.at(line) = line;
            table.columns.at(col).at(line) = c[col].data[line];
        }
    }

    KDTree index = FullKDTree(table);
    end = std::chrono::system_clock::now();

    response_times->at(0) += std::chrono::duration<double>(end - start).count();

    for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index)
    {
        // Transform query in a format easier to handle
        std::vector<std::pair<int64_t, int64_t>> query(NUMBER_OF_COLUMNS);
        for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
        {
            query.at(i).first = rangequeries[i].leftpredicate[query_index];
            query.at(i).second = rangequeries[i].rightpredicate[query_index];
        }

        start = std::chrono::system_clock::now();
        std::vector<int64_t> result = SearchKDTree(index, query, table, false);
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
    for (int i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        free(c[i].data);
        free(rangequeries[i].leftpredicate);
        free(rangequeries[i].rightpredicate);
    }

    free(c);
    free(rangequeries);
}

//.column.txt .query.txt 10 10000 0 2
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

    //FULL SCAN
    if (INDEXING_TYPE == 0)
    {
        std::vector<double> fullscantime(NUM_QUERIES);
        full_scan(&fullscantime);
        for (int q = 0; q < NUM_QUERIES; q++)
            std::cout << fullscantime[q] << "\n";
    }

    // STANDARD CRACKING W/ AVL
    else if (INDEXING_TYPE == 1)
    {
        std::vector<double> standardcracking(NUM_QUERIES);
        standardCracking(&standardcracking);
        for (int q = 0; q < NUM_QUERIES; q++)
            std::cout << standardcracking[q] << "\n";
    }

    // FULL INDEX B+ Tree
    else if (INDEXING_TYPE == 2)
    {
        BPTREE_ELEMENTSPERNODE = atoi(argv[7]);
        std::vector<double> fullindex(NUM_QUERIES);
        bptree_bulk_index3(&fullindex);
        for (int q = 0; q < NUM_QUERIES; q++)
            std::cout << fullindex[q] << "\n";
    }

    //  Cracking W/ KD-Tree
    else if (INDEXING_TYPE == 3)
    {
        KDTREE_THRESHOLD = atoi(argv[7]);
        std::vector<double> kdtree(NUM_QUERIES);
        kdtree_cracking(&kdtree);
        for (int q = 0; q < NUM_QUERIES; q++)
            std::cout << kdtree[q] << "\n";
    }

    // Full Index KD-TREE
    else if (INDEXING_TYPE == 4)
    {
        KDTREE_THRESHOLD = atoi(argv[7]);
        std::vector<double> kdtree(NUM_QUERIES);
        full_kdtree_cracking(&kdtree);
        for (int q = 0; q < NUM_QUERIES; q++)
            std::cout << kdtree[q] << "\n";
    }
}
