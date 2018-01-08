//
// Created by PHolanda on 17/12/17.
//
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <string>

#include "fullindex/bulkloading_bp_tree.h"
#include "cracking/avl_tree.h"
#include "cracking/standard_cracking.h"
#include "util/file_manager.h"
#include "fullindex/hybrid_radix_insert_sort.h"
#include "util/structs.h"

#define VERIFY

std::string COLUMN_FILE_PATH ,  QUERIES_FILE_PATH ;
extern int64_t  COLUMN_SIZE, BPTREE_ELEMENTSPERNODE;
int64_t NUM_QUERIES , NUMBER_OF_REPETITIONS, NUMBER_OF_COLUMNS;

int64_t range_query_baseline(const std::vector<int64_t>& array, int64_t min_bounds, int64_t max_bounds) {
    int64_t sum = 0;
    for(size_t i = 0; i < array.size(); i++) {
        if (array[i] >= min_bounds && array[i] < max_bounds) {
            sum += array[i];
        }
    }
    return sum;
}


bool verify_range_query(Column& c, int64_t l, int64_t r, int64_t sum) {
    int64_t r1 = range_query_baseline(c.data, l, r);
    if (sum != r1) {
        fprintf(stderr, "Incorrect Results Expected: %lld  Got: %lld \n", r1, sum);
        assert(0);
        return false;
    }
    return true;
}

int64_t scanQuery(IndexEntry *c, int64_t from, int64_t to){
    int64_t  sum = 0;
    for(int64_t i = from;i<=to;i++) {
        sum += c[i].m_key;
    }

    return sum;
}

void standardCracking(std::vector<double> * standardcrackingtime) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;
    RangeQuery rangequeries;
    loadQueries(&rangequeries,QUERIES_FILE_PATH,NUM_QUERIES);
    Column c;
    c.data = std::vector<int64_t>(COLUMN_SIZE);
    loadcolumn(&c,COLUMN_FILE_PATH,COLUMN_SIZE);
    start = std::chrono::system_clock::now();
    IndexEntry *crackercolumn = (IndexEntry *) malloc(COLUMN_SIZE * 2 * sizeof(int64_t));
    //Creating Cracker Column
    for (size_t i = 0; i < COLUMN_SIZE; i++) {
        crackercolumn[i].m_key = c.data[i];
        crackercolumn[i].m_rowId = i;
    }
    //Initialitizing Cracker Index
    AvlTree T = NULL;
    end = std::chrono::system_clock::now();
    standardcrackingtime->at(0) +=   std::chrono::duration<double>(end - start).count();

    for (size_t i = 0; i < NUM_QUERIES; i++) {
        start = std::chrono::system_clock::now();
        //Partitioning Column and Inserting in Cracker Indexing
        T = standardCracking(crackercolumn , COLUMN_SIZE, T, rangequeries.leftpredicate[i], rangequeries.rightpredicate[i]);
        //Querying
        IntPair p1 = FindNeighborsGTE(rangequeries.leftpredicate[i], (AvlTree)T, COLUMN_SIZE-1);
        IntPair p2 = FindNeighborsLT(rangequeries.rightpredicate[i], (AvlTree)T, COLUMN_SIZE-1);
        int offset1 = p1->first;
        int offset2 = p2->second;
        free(p1);
        free(p2);
        int64_t sum = scanQuery(crackercolumn, offset1, offset2);
        end = std::chrono::system_clock::now();
        standardcrackingtime->at(i) += std::chrono::duration<double>(end - start).count();
#ifdef VERIFY
        bool pass = verify_range_query(c,rangequeries.leftpredicate[i],rangequeries.rightpredicate[i],sum);
                if (pass == 0) std::cout << "Query : " << i <<" " <<  pass << "\n";
#endif

    }
    free(crackercolumn);
}

//c[i].m_key >= keyL && c[i].m_key < keyH
long filterQuery3(IndexEntry **c, RangeQuery* queries, size_t query_index, int64_t from, int64_t to){
    int64_t sum = 0;
    for (int i = from; i < to; ++i) {
        bool is_valid = true;
        int64_t partial_sum = 0;
        for (int j = 0; j < NUMBER_OF_COLUMNS && is_valid; ++j) {
            int64_t keyL = queries[j].leftpredicate[query_index];
            int64_t keyH = queries[j].rightpredicate[query_index];
            if(!(c[j][i].m_key >= keyL && c[j][i] < keyH)){
                is_valid = false;
            }else{
                partial_sum += c[j][i].m_key;
            }
        }
        if(is_valid){
            sum += partial_sum;
        }
    }
    return sum;
}

void full_scan(std::vector<double> * fullscantime){
    std::chrono::time_point<std::chrono::system_clock> start, end;

    RangeQuery *rangequeries = (RangeQuery *) malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

    Column *c = (Column*) malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i) {
        c[i].data = std::vector<int64_t>(COLUMN_SIZE);
    }
    loadcolumn(c,COLUMN_FILE_PATH,COLUMN_SIZE, NUMBER_OF_COLUMNS);
    IndexEntry **crackercolumns = (IndexEntry **) malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry*));
    for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j) {
        crackercolumns[j] = (IndexEntry *) malloc(COLUMN_SIZE * sizeof(IndexEntry));
        // Already create the cracker column
        for (size_t i = 0; i < COLUMN_SIZE; ++i) {
            crackercolumns[j][i].m_key = c[j].data[i];
            crackercolumns[j][i].m_rowId = i;
        }
    }
    for(int q=0;q<NUM_QUERIES;q++){
        start = std::chrono::system_clock::now();
        int64_t sum = filterQuery3(crackercolumns, rangequeries, q, 0, COLUMN_SIZE-1);
        end = std::chrono::system_clock::now();
        fullscantime->at(q) += std::chrono::duration<double>(end - start).count();
#ifdef VERIFY
        bool pass = verify_range_query(c,rangequeries.leftpredicate[q],rangequeries.rightpredicate[q],sum);
                if (pass == 0) std::cout << "Query : " << q <<" " <<  pass << "\n";
#endif
    }
//    free(crackercolumn);

}

void *fullIndex(IndexEntry *c){
    hybrid_radixsort_insert(c, COLUMN_SIZE);
    void *I = build_bptree_bulk(c, COLUMN_SIZE);

    return I;
}

void bptree_bulk_index3(std::vector<double> * fullindex){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    RangeQuery rangequeries;
    loadQueries(&rangequeries,QUERIES_FILE_PATH,NUM_QUERIES);
    Column c;
    c.data = std::vector<int64_t>(COLUMN_SIZE);
    loadcolumn(&c,COLUMN_FILE_PATH,COLUMN_SIZE);
    IndexEntry *data = (IndexEntry *) malloc(COLUMN_SIZE * 2 * sizeof(int64_t));
    for (size_t i = 0; i < COLUMN_SIZE; i++) {
        data[i].m_key = c.data[i];
        data[i].m_rowId = i;
    }
    start = std::chrono::system_clock::now();
    BulkBPTree* T = (BulkBPTree*) fullIndex(data);
    end = std::chrono::system_clock::now();
    fullindex->at(0) += std::chrono::duration<double>(end - start).count();
    for(int i=0;i<NUM_QUERIES;i++){
        // query
        start = std::chrono::system_clock::now();
        int64_t offset1 = (T)->gte(rangequeries.leftpredicate[i]);
        int64_t offset2 = (T)->lt(rangequeries.rightpredicate[i]);
        int64_t sum = scanQuery(data, offset1, offset2);
        end = std::chrono::system_clock::now();
        fullindex->at(i) += std::chrono::duration<double>(end - start).count();
#ifdef VERIFY
        bool pass = verify_range_query(c,rangequeries.leftpredicate[i],rangequeries.rightpredicate[i],sum);
            if (pass == 0) std::cout << "Query : " << i <<" " <<  pass << "\n";
#endif

    }
    free(data);
    free(T);
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
//        STANDARD CRACKING
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
        BPTREE_ELEMENTSPERNODE= atoi(argv[7]);
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


}
