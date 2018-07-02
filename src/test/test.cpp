#include "test.h"

extern int64_t COLUMN_SIZE,NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD, INDEXING_TYPE;

using namespace std;


vector<vector<int64_t>> range_query_baseline(Table *table, vector< vector<pair<int64_t, int64_t>>> *queries)
{
    vector<vector<int64_t>> queryResult;
    for (size_t q = 0; q < NUM_QUERIES; ++ q){
        vector<int64_t> rowId;
        for (size_t i = 0; i < COLUMN_SIZE; ++i){
            // fprintf(stderr, "here" );
            bool match = true;
            for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j){
                if(table->columns[j][i] < queries->at(q).at(j).first|| table->columns[j][i] >= queries->at(q).at(j).second){
                    match = false;
                    break;
                }
            }
        if(match)
            rowId.push_back(table->ids[i]);
        }
        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }

	return queryResult;
}

vector<vector<int64_t>> vectorized_branchless_full_scan(Table *table, vector< vector<pair<int64_t, int64_t>>> *queries)
{
    vector<vector<int64_t>> queryResult;
    size_t vector_size = 2000; // 2000*64 = 128000 bits 1/2 L1.
    size_t sel_size;
    int sel_vector [vector_size];
    for (size_t q = 0; q < NUM_QUERIES; ++ q){
        vector<int64_t> rowId;
        for (size_t i = 0; i < COLUMN_SIZE/vector_size; ++ i){
            sel_size = select_rq_scan_new (sel_vector, &table->columns[0][vector_size*i],queries->at(q).at(0).first,queries->at(q).at(0).second,vector_size);
            for (size_t column_num = 1; column_num < NUMBER_OF_COLUMNS; column_num++)
                sel_size = select_rq_scan_sel_vec(sel_vector, &table->columns[column_num][vector_size*i],queries->at(q).at(column_num).first,queries->at(q).at(column_num).second,sel_size);
            for(size_t j = 0; j < sel_size; ++ j)
                rowId.push_back(table->ids[vector_size*i+sel_vector[j]]);
    }
        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

vector<vector<int64_t>> unidimensional_cracking(Table *table, vector< vector<pair<int64_t, int64_t>>> *queries)
{
    vector<vector<int64_t>> queryResult;
    table->crackercolumns = (IndexEntry **)malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
    Tree *T = (Tree *)malloc(sizeof(Tree) * NUMBER_OF_COLUMNS);
    vector<boost::dynamic_bitset<>> bitmaps(NUMBER_OF_COLUMNS);
    cracking_pre_processing(table,T);
    for (size_t q = 0; q < NUM_QUERIES; ++ q){
        cracking_partial_built(table,T,&rangeQueries.at(i));
        cracking_index_lookup(T,&rangeQueries.at(i),&offsets);
        cracking_intersection(Table *table,vector<pair<int,int>>  *offsets, vector<boost::dynamic_bitset<>> *bitmaps, int64_t * result);
    }





    size_t vector_size = 2000; // 2000*64 = 128000 bits 1/2 L1.
    size_t sel_size;
    int sel_vector [vector_size];
    for (size_t q = 0; q < NUM_QUERIES; ++ q){
        vector<int64_t> rowId;
        for (size_t i = 0; i < COLUMN_SIZE/vector_size; ++ i){
            sel_size = select_rq_scan_new (sel_vector, &table->columns[0][vector_size*i],queries->at(q).at(0).first,queries->at(q).at(0).second,vector_size);
            for (size_t column_num = 1; column_num < NUMBER_OF_COLUMNS; column_num++)
                sel_size = select_rq_scan_sel_vec(sel_vector, &table->columns[column_num][vector_size*i],queries->at(q).at(column_num).first,queries->at(q).at(column_num).second,sel_size);
            for(size_t j = 0; j < sel_size; ++ j)
                rowId.push_back(table->ids[vector_size*i+sel_vector[j]]);
    }
        sort(rowId.begin(),rowId.end());
        queryResult.push_back(rowId);
    }
    return queryResult;
}

void verify_range_query(vector<vector<int64_t>> queryResultBaseline,vector<vector<int64_t>> queryResultToBeTested)
{
    for (size_t i = 0; i < queryResultBaseline.size(); ++i)
        for(size_t j = 0; j < queryResultBaseline.at(i).size(); ++ j)
        	if (queryResultBaseline.at(i).at(j) != queryResultToBeTested.at(i).at(j))
        	{
        		fprintf(stderr, "Incorrect Results!\n");
        		fprintf(stderr, "Query: %ld\n", i);
        		fprintf(stderr, "Expected: %ld\n Got: %ld\n", queryResultBaseline.at(i).at(j), queryResultToBeTested.at(i).at(j));
        		assert(0);
        	}
}



void verifyAlgorithms(Table *table, vector<vector<pair<int64_t,int64_t>>> rangeQueries){
    vector<vector<int64_t>> queryResultToBeTested;
    fprintf(stderr, "Running Baseline.\n");
    vector<vector<int64_t>> queryResultBaseline = range_query_baseline(table,&rangeQueries);
    fprintf(stderr, "Running Vectorized Branchless Scan.\n");
    queryResultToBeTested = vectorized_branchless_full_scan(table,&rangeQueries);
    verify_range_query(queryResultBaseline,queryResultToBeTested);
    fprintf(stderr, "Running Unidimensional Cracking.\n");

    // verify_range_query(c, range_query_incremental_mergesort, delta);
    // printf("Running \n");
    // verify_range_query(c, range_query_incremental_mergesort_branched, delta);
    // printf("Running Unidimensional Cracking.\n");
    // verify_range_query(c, range_query_incremental_quicksort, delta);
    // printf("Running Cracking KD-Tree.\n");
    // verify_range_query(c, range_query_incremental_quicksort_branched, delta);
    // printf("Running Full KD-Tree.\n");
    // verify_range_query(c, range_query_incremental_bucketsort_equiwidth, delta);
}
