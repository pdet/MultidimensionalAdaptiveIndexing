//
// Created by PHolanda on 17/12/17.
//
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <map>
#include <string>

#include "fullindex/bulkloading_bp_tree.h"
#include "cracking/avl_tree.h"
#include "cracking/kd_tree.h"
#include "cracking/standard_cracking.h"
#include "util/file_manager.h"
#include "fullindex/hybrid_radix_insert_sort.h"
#include "util/structs.h"

//#define VERIFY

std::string COLUMN_FILE_PATH ,  QUERIES_FILE_PATH ;
extern int64_t  COLUMN_SIZE, BPTREE_ELEMENTSPERNODE;
int64_t NUM_QUERIES , NUMBER_OF_REPETITIONS, NUMBER_OF_COLUMNS;

int64_t range_query_baseline(Column* c, RangeQuery* queries, size_t query_index) {
    int64_t sum = 0;
    for (size_t i = 0; i <= COLUMN_SIZE-1; ++i) {
        bool is_valid = true;
        int64_t partial_sum = 0;
        for (size_t j = 0; j < NUMBER_OF_COLUMNS && is_valid; ++j) {
            int64_t keyL = queries[j].leftpredicate[query_index];
            int64_t keyH = queries[j].rightpredicate[query_index];
            if(!(c[j].data[i] >= keyL && c[j].data[i] < keyH)){
                is_valid = false;
            }else{
                partial_sum += c[j].data[i];
            }
        }
        if(is_valid){
            sum += partial_sum;
        }
    }
    return sum;
}


bool verify_range_query(Column* c, RangeQuery* queries, size_t query_index, int64_t sum) {
    int64_t r1 = range_query_baseline(c, queries, query_index);
    if (sum != r1) {
        fprintf(stderr, "Incorrect Results Expected: %lld  Got: %lld \n", r1, sum);
        assert(0);
        return false;
    }
    return true;
}

std::vector<IndexEntry> scanQuery(IndexEntry *c, int64_t from, int64_t to){
    std::vector<IndexEntry> result (to - from + 1);
    for(size_t i = from;i<=to;i++) {
        result.at(i-from).m_key = c[i].m_key;
        result.at(i-from).m_rowId = c[i].m_rowId;
    }

    return result;
}

std::multimap<int64_t, bool> join_results(std::vector<std::vector<IndexEntry>> partials){
    std::multimap<int64_t, bool> intersection;
    // Copy the first partial IDs
    for (size_t j = 0; j < partials[0].size(); ++j) {
        intersection.insert(std::make_pair(partials[0][j].m_rowId, true));
    }

    for (size_t i = 1; i < partials.size(); ++i) {
        std::multimap<int64_t, bool> tmp_intersection;
        for (size_t j = 0; j < partials[i].size(); ++j) {
            int64_t id = partials[i][j].m_rowId;
            // Check if id is inside intersection
            if(intersection.find(id) != intersection.end()){
                tmp_intersection.insert(std::make_pair(id, true));
            }
        }
        intersection = tmp_intersection;
    }

    return intersection;
}

int64_t sum_result(std::multimap<int64_t, bool> result, std::vector<std::vector<IndexEntry>> partials){
    // Find the resulting rows and sum them
    int64_t sum = 0;
    std::multimap<int64_t, bool>::iterator it;
    for (it = result.begin(); it != result.end(); it++) {
        int64_t id = it->first;
        for (size_t i = 0; i < partials.size(); ++i) {
            for (size_t j = 0; j < partials[i].size(); ++j){
                if(id == partials[i][j].m_rowId){
                    sum += partials[i][j].m_key;
                }
            }
        }
    }
    return sum;
}

void standardCracking(std::vector<double> * standardcrackingtime) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;

    Column *c = (Column*) malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c,COLUMN_FILE_PATH,COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *) malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    start = std::chrono::system_clock::now();
    IndexEntry **crackercolumns = (IndexEntry **) malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry*));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j) {
        crackercolumns[j] = (IndexEntry *) malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i) {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }
    //Initialitizing multiple Cracker Indexes
    AvlTree *T = (AvlTree *) malloc(sizeof(AvlTree) * NUMBER_OF_COLUMNS);
    for (size_t k = 0; k < NUMBER_OF_COLUMNS; ++k) {
        T[k] = NULL;
    }
    end = std::chrono::system_clock::now();
    standardcrackingtime->at(0) +=   std::chrono::duration<double>(end - start).count();


    for (size_t i = 0; i < NUM_QUERIES; i++) {
        start = std::chrono::system_clock::now();
        std::vector<std::vector<IndexEntry>> partial_results (NUMBER_OF_COLUMNS);
        for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j) {
            //Partitioning Column and Inserting in Cracker Indexing

            T[j] = standardCracking(
                    crackercolumns[j],
                    COLUMN_SIZE,
                    T[j],
                    rangequeries[j].leftpredicate[i],
                    rangequeries[j].rightpredicate[i]
            );

            //Querying

            IntPair p1 = FindNeighborsGTE(rangequeries[j].leftpredicate[i], T[j], COLUMN_SIZE-1);
            IntPair p2 = FindNeighborsLT(rangequeries[j].rightpredicate[i], T[j], COLUMN_SIZE-1);
            int offset1 = p1->first;
            int offset2 = p2->second;
            free(p1);
            free(p2);

            partial_results[j] = scanQuery(crackercolumns[j], offset1, offset2);
        }
        // Join the partial results
        std::multimap<int64_t, bool> result = join_results(partial_results);

        end = std::chrono::system_clock::now();
        standardcrackingtime->at(i) += std::chrono::duration<double>(end - start).count();

#ifdef VERIFY
        int64_t sum = sum_result(result, partial_results);
        bool pass = verify_range_query(c,rangequeries, i, sum);
        if (pass == 0) std::cout << "Query : " << i <<" " <<  pass << "\n";
#endif
    }
    for (size_t l = 0; l < NUMBER_OF_COLUMNS; ++l) {
        free(crackercolumns[l]);
    }
    free(crackercolumns);
}

std::vector<IndexEntry> filterQuery3(IndexEntry *c, RangeQuery queries, size_t query_index, int64_t from, int64_t to){
    std::vector<IndexEntry> results;
    for (size_t i = from; i <= to; ++i) {
        int64_t keyL = queries.leftpredicate[query_index];
        int64_t keyH = queries.rightpredicate[query_index];
        if(c[i].m_key >= keyL && c[i].m_key < keyH){
            results.push_back(IndexEntry(c[i].m_key, c[i].m_rowId));
        }
    }
    return results;
}

void full_scan(std::vector<double> * fullscantime){
    std::chrono::time_point<std::chrono::system_clock> start, end;

    Column *c = (Column*) malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c,COLUMN_FILE_PATH,COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *) malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    IndexEntry **crackercolumns = (IndexEntry **) malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry*));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j) {
        crackercolumns[j] = (IndexEntry *) malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i) {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }
    for(size_t q=0;q<NUM_QUERIES;q++){
        start = std::chrono::system_clock::now();
        std::vector<std::vector<IndexEntry>> partial_results (NUMBER_OF_COLUMNS);
        for (int i = 0; i < NUMBER_OF_COLUMNS; ++i) {
            partial_results[i] = filterQuery3(crackercolumns[i], rangequeries[i], q, 0, COLUMN_SIZE-1);
        }
        // Join the partial results
        std::multimap<int64_t, bool> result = join_results(partial_results);

        end = std::chrono::system_clock::now();
        fullscantime->at(q) += std::chrono::duration<double>(end - start).count();
#ifdef VERIFY
        int64_t sum = sum_result(result, partial_results);
        bool pass = verify_range_query(c,rangequeries, i, sum);
        if (pass == 0) std::cout << "Query : " << i <<" " <<  pass << "\n";
#endif
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i) {
        free(crackercolumns[i]);
    }
    free(crackercolumns);
}

void *fullIndex(IndexEntry *c){
    hybrid_radixsort_insert(c, COLUMN_SIZE);
    void *I = build_bptree_bulk(c, COLUMN_SIZE);

    return I;
}

void bptree_bulk_index3(std::vector<double> * fullindex){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Column *c = (Column*) malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c,COLUMN_FILE_PATH,COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *) malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    IndexEntry **crackercolumns = (IndexEntry **) malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry*));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j) {
        crackercolumns[j] = (IndexEntry *) malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i) {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }

    start = std::chrono::system_clock::now();
    BulkBPTree** T = (BulkBPTree **) malloc(sizeof(BulkBPTree *) * NUMBER_OF_COLUMNS);
    for (size_t k = 0; k < NUMBER_OF_COLUMNS; ++k) {
        T[k] = (BulkBPTree*) fullIndex(crackercolumns[k]);
    }
    end = std::chrono::system_clock::now();
    fullindex->at(0) += std::chrono::duration<double>(end - start).count();

    for(int i=0;i<NUM_QUERIES;i++){
        // query
        start = std::chrono::system_clock::now();
        std::vector<std::vector<IndexEntry>> partial_results (NUMBER_OF_COLUMNS);
        for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j) {
            int64_t offset1 = (T[j])->gte(rangequeries[j].leftpredicate[i]);
            int64_t offset2 = (T[j])->lt(rangequeries[j].rightpredicate[i]);
            partial_results[j] = scanQuery(crackercolumns[j], offset1, offset2);
        }
        std::multimap<int64_t, bool> result = join_results(partial_results);
        end = std::chrono::system_clock::now();
        fullindex->at(i) += std::chrono::duration<double>(end - start).count();

#ifdef VERIFY
        int64_t sum = sum_result(result, partial_results);
        bool pass = verify_range_query(c,rangequeries, i, sum);
        if (pass == 0) std::cout << "Query : " << i <<" " <<  pass << "\n";
#endif

    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i) {
        free(crackercolumns[i]);
        free(T[i]);
    }
    free(crackercolumns);
    free(T);
}


void kdtree_cracking(std::vector<double> *response_times) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Column *c = (Column*) malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c,COLUMN_FILE_PATH,COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *) malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    IndexEntry **crackercolumns = (IndexEntry **) malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry*));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j) {
        crackercolumns[j] = (IndexEntry *) malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i) {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }

    // Copy table contents to Index
    start = std::chrono::system_clock::now();
    KDTree index = InitializeKDTree(COLUMN_SIZE, NUMBER_OF_COLUMNS, crackercolumns);
    end = std::chrono::system_clock::now();
    response_times->at(0) += std::chrono::duration<double>(end - start).count();

    for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index) {
        start = std::chrono::system_clock::now();
        //Do Query
        end = std::chrono::system_clock::now();
        response_times->at(query_index) += std::chrono::duration<double>(end - start).count();
    }

}
int main(int argc, char** argv) {
    int INDEXING_TYPE;

    if (argc < 7) {
        printf("Missing mandatory parameters\n");
        return -1;
    }
    COLUMN_FILE_PATH =  argv[1];
    QUERIES_FILE_PATH = argv[2];
    NUM_QUERIES = std::stoi(argv[3]);
    NUMBER_OF_REPETITIONS =atoi(argv[4]);
    COLUMN_SIZE = atoi(argv[5]);
    INDEXING_TYPE= atoi(argv[6]);
    NUMBER_OF_COLUMNS = atoi(argv[7]);
    //FULL SCAN
    if (INDEXING_TYPE == 0) {
        std::vector<double> fullscantime(NUM_QUERIES);
        for (int i = 0; i < NUMBER_OF_REPETITIONS; i++){
            fprintf(stderr, "Repetition #%d\n", i);
            full_scan(&fullscantime);
        }

        for (int q = 0; q < NUM_QUERIES; q++) {
            fullscantime[q] = fullscantime[q]/NUMBER_OF_REPETITIONS;
            std::cout  << fullscantime[q] << "\n";
        }
    }
////        STANDARD CRACKING
    else if (INDEXING_TYPE == 1){
        std::vector<double> standardcracking(NUM_QUERIES);
        for (int i = 0; i < NUMBER_OF_REPETITIONS; i++){
            fprintf(stderr, "Repetition #%d\n", i);
            standardCracking(&standardcracking);
        }

        for (int q = 0; q < NUM_QUERIES; q++) {
            standardcracking[q] = standardcracking[q]/NUMBER_OF_REPETITIONS;
            std::cout  << standardcracking[q] << "\n";
        }
    }

//        FULL INDEX
    else if (INDEXING_TYPE == 2){
        BPTREE_ELEMENTSPERNODE= atoi(argv[8]);
        std::vector<double> fullindex(NUM_QUERIES);

        for (int i = 0; i < NUMBER_OF_REPETITIONS; i++){
            fprintf(stderr, "Repetition #%d\n", i);
            bptree_bulk_index3(&fullindex);
        }

        for (int q = 0; q < NUM_QUERIES; q++) {
            fullindex[q] = fullindex[q]/NUMBER_OF_REPETITIONS;
            std::cout  << fullindex[q] << "\n";

        }
    }

//        KD-TREE
    else if (INDEXING_TYPE == 3){
        std::vector<double> kdtree(NUM_QUERIES);

        for (int i = 0; i < NUMBER_OF_REPETITIONS; i++){
            fprintf(stderr, "Repetition #%d\n", i);
            kdtree_cracking(&kdtree);
        }

        for (int q = 0; q < NUM_QUERIES; q++) {
            kdtree[q] = kdtree[q]/NUMBER_OF_REPETITIONS;
            std::cout  << kdtree[q] << "\n";

        }

    }


}
