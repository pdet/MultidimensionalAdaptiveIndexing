#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>

#include "cracking/avl_tree.h"

#include "cracking/kd_tree.h"
#include "cracking/standard_cracking.h"
#include "util/file_manager.h"
#include "util/structs.h"
#include "util/util.h"
#include "test/test.h"
#include "util/define.h"

// #define test

using namespace std;

typedef void (*pre_processing_function)(Table *table, Tree * t);
typedef void (*partial_index_built_function)(Table *table, Tree * T,vector<array<int64_t, 3>>  *rangequeries);
typedef void (*index_lookup_function)(Tree * T,vector<array<int64_t, 3>>  *rangequeries,vector<pair<int,int>>  *offsets);
typedef void (*scan_data_function)(Table *table, vector<array<int64_t, 3>>  *rangequeries,vector<pair<int,int>>  *offsets, vector<int64_t> * result);
typedef void (*intersect_data_function)(Table *table,vector<pair<int,int>>  *offsets, vector<vector<bool> > *bitmaps, vector<int64_t> * result);

//Settings for Indexes
const int FULL_SCAN = 0;
const int UNIDIMENSIONAL_CRACKING = 1;
const int CRACKING_KDTREE = 2;
const int KDTREE = 3;
const int SIDEWAYS_CRACKING = 4;

string DATA_FILE_PATH, QUERIES_FILE_PATH;
int64_t COLUMN_SIZE,NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD, INDEXING_TYPE;

void benchmarkFunction(Table *table, vector<vector<array<int64_t, 3>>> rangeQueries, 
	pre_processing_function pre_processing, partial_index_built_function partial_index_built,
	index_lookup_function index_lookup, scan_data_function scan_data, intersect_data_function intersect_data){

	vector<double> indexCreation = vector<double>(NUM_QUERIES, 0);
	vector<double> indexLookup = vector<double>(NUM_QUERIES, 0);
	vector<double> scanTime = vector<double>(NUM_QUERIES, 0);
	vector<double> joinTime = vector<double>(NUM_QUERIES, 0);
    vector<double> projectionTime = vector<double>(NUM_QUERIES, 0);
	vector<double> totalTime = vector<double>(NUM_QUERIES);
	chrono::time_point<chrono::system_clock> start, end;
	
	table->crackercolumns = (IndexEntry **)malloc(NUMBER_OF_COLUMNS * sizeof(IndexEntry *));
	Tree *T;
	if(INDEXING_TYPE == 1)
		T = (Tree *)malloc(sizeof(Tree) * NUMBER_OF_COLUMNS);
	else 
		T = (Tree *)malloc(sizeof(Tree));

	vector<vector<bool> > bitmaps(NUMBER_OF_COLUMNS);
	// First we do pre-processing. In the case of full index we fully create the index.
	// In the case of Partial Indexes we copy the elements to a cracker index structure.
    start = chrono::system_clock::now();

    if(pre_processing)
    	pre_processing(table,T);
    end = chrono::system_clock::now();
    indexCreation.at(0)  = chrono::duration<double>(end - start).count();

    for(int i = 0; i < NUM_QUERIES; i++) {
		vector<pair<int, int>> offsets;  
    	vector<int64_t> result;
    	// If we are running cracking algorithms we do a partial index creation step
    	start = chrono::system_clock::now();

    	if(partial_index_built)
    		partial_index_built(table,T,&rangeQueries.at(i));
    	end = chrono::system_clock::now();
        indexCreation.at(i) += chrono::duration<double>(end - start).count();

        start = chrono::system_clock::now();
        if(index_lookup)
    		index_lookup(T,&rangeQueries.at(i),&offsets);
    	end = chrono::system_clock::now();
        indexLookup.at(i) = chrono::duration<double>(end - start).count();
      
       // Intersecting data for uni-dimensional indexes
        start = chrono::system_clock::now();
    	if (scan_data)
    		scan_data(table,&rangeQueries.at(i),&offsets,&result);
    	end = chrono::system_clock::now();
        scanTime.at(i)  = chrono::duration<double>(end - start).count();
        
        start = chrono::system_clock::now(); 
    	if(intersect_data)
    		intersect_data(table,&offsets,&bitmaps,&result);
    	end = chrono::system_clock::now();
        joinTime.at(i)  = chrono::duration<double>(end - start).count();

        start = chrono::system_clock::now(); 
    	int64_t final_result = tpchProjection(table, &result);
    	end = chrono::system_clock::now();
        projectionTime.at(i)  = chrono::duration<double>(end - start).count();

        totalTime.at(i)  = indexCreation.at(i) + indexLookup.at(i) + scanTime.at(i) + joinTime.at(i) + projectionTime.at(i);
        fprintf(stderr, "Result : %lu\n",final_result);
    }
	for (int i = 0; i < NUM_QUERIES; i++){
		cout << indexCreation.at(i) << ";" << indexLookup.at(i) << ";" << scanTime.at(i) << ";" << joinTime.at(i) << ";" << totalTime.at(i) << ";" << projectionTime.at(i) << "\n";
		fprintf(stderr, "%f\n",totalTime.at(i));
	}
}

int main(int argc, char **argv)
{
	// Default Values
    NUM_QUERIES = 1000;
	KDTREE_THRESHOLD = 1000;
    DATA_FILE_PATH = "./benchmarks/TPCH/lineitem.csv";
    QUERIES_FILE_PATH = "./benchmarks/TPCH/queries.csv";

    for(int i = 1; i < argc; i++) {
        auto arg = string(argv[i]);
        if (arg.substr(0,2) != "--") {
            print_help(argc, argv);
            exit(EXIT_FAILURE);
        }
        arg = arg.substr(2);
        auto p = split_once(arg, '=');
        auto& arg_name = p.first; auto& arg_value = p.second;
        if (arg_name == "column-path") {
            DATA_FILE_PATH = arg_value;
        } else if (arg_name == "query-path") {
            QUERIES_FILE_PATH = arg_value;
        }  else if (arg_name == "num-queries") {
            NUM_QUERIES = stoi(arg_value.c_str());
        } else if (arg_name == "kdtree-threshold") {
            KDTREE_THRESHOLD = atoi(arg_value.c_str());
        } else if (arg_name == "indexing-type") {
            INDEXING_TYPE = atoi(arg_value.c_str());
        } else {
            print_help(argc, argv);
            exit(EXIT_FAILURE);
        }
    }

    Table table;
    tpch_loadData(table, DATA_FILE_PATH);

    vector<vector<array<int64_t, 3>>> query;
    tpch_loadQueries(query, QUERIES_FILE_PATH);

    NUMBER_OF_COLUMNS = table.columns.size();
    COLUMN_SIZE = table.ids.size();

    #ifdef test
        verifyAlgorithms(&table,query);
    #else
    	switch(INDEXING_TYPE){
    		case FULL_SCAN:
    			benchmarkFunction(&table,query,NULL,NULL,NULL,full_scan,NULL);
    			break;
    		case UNIDIMENSIONAL_CRACKING:
    			benchmarkFunction(&table,query,cracking_pre_processing,cracking_partial_built,cracking_index_lookup,NULL,cracking_intersection);
    			break;
    		case CRACKING_KDTREE:
    			benchmarkFunction(&table,query,cracking_kdtree_pre_processing,cracking_kdtree_partial_built,kdtree_index_lookup,kdtree_scan,NULL);
    			break;
    		case KDTREE:
    			benchmarkFunction(&table,query,full_kdtree_pre_processing,NULL,kdtree_index_lookup,kdtree_scan,NULL);
    			break;
            case SIDEWAYS_CRACKING:
                benchmarkFunction(&table,query,sideways_cracking_pre_processing,sideways_cracking_partial_built,sideways_cracking_index_lookup,sideways_cracking_scan,NULL);
                break;
    	}
    #endif
}
