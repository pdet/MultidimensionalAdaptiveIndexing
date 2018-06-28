//
// Created by PHolanda on 17/12/17.
//
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

#include "cracking/avl_tree.h"

#include "cracking/kd_tree.h"
#include "cracking/standard_cracking.h"
#include "fullindex/bulkloading_bp_tree.h"
#include "fullindex/hybrid_radix_insert_sort.h"
#include "util/file_manager.h"
#include "util/structs.h"
#include "util/timer.h"
#include "util/util.h"

using namespace std;

typedef int (*scan_data_function)(Column *c, vector<pair<int64_t,int64_t>>  *rangequeries);


string COLUMN_FILE_PATH, QUERIES_FILE_PATH;
int64_t COLUMN_SIZE, BPTREE_ELEMENTSPERNODE;
int64_t NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD, INDEXING_TYPE;

vector<double> indexCreation;
vector<double> indexLookup;
vector<double> scanTime;
vector<double> joinTime;
vector<double> totalTime;


void standardCracking()
{
	chrono::time_point<chrono::system_clock> start, end;

	Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
	loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

	RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
	loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

	start = chrono::system_clock::now();
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
	end = chrono::system_clock::now();
	indexCreation.at(0) += chrono::duration<double>(end - start).count();

	for (size_t i = 0; i < NUM_QUERIES; i++)
	{
		vector<boost::dynamic_bitset<>> bitmaps(NUMBER_OF_COLUMNS);
		for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
		{
			//Partitioning Column and Inserting in Cracker Indexing
			start = chrono::system_clock::now();
			bitmaps.at(j).resize(COLUMN_SIZE);
			T[j] = standardCracking(
				crackercolumns[j],
				COLUMN_SIZE,
				T[j],
				rangequeries[j].leftpredicate[i],
				rangequeries[j].rightpredicate[i]);
			end = chrono::system_clock::now();
			indexCreation.at(i) += chrono::duration<double>(end - start).count();
			start = chrono::system_clock::now();
			//Querying
			IntPair p1 = FindNeighborsGTE(rangequeries[j].leftpredicate[i], T[j], COLUMN_SIZE - 1);
			IntPair p2 = FindNeighborsLT(rangequeries[j].rightpredicate[i], T[j], COLUMN_SIZE - 1);
			int offset1 = p1->first;
			int offset2 = p2->second;
			free(p1);
			free(p2);
			end = chrono::system_clock::now();
			indexLookup.at(i) += chrono::duration<double>(end - start).count();
			start = chrono::system_clock::now();
			scanQuery(crackercolumns[j], offset1, offset2, bitmaps[j]);
			end = chrono::system_clock::now();
			scanTime.at(i) += chrono::duration<double>(end - start).count();
		}
		int64_t result;
		start = std::chrono::system_clock::now();
		result = join_bitmaps(bitmaps, c);
		end = std::chrono::system_clock::now();
		joinTime.at(i) += std::chrono::duration<double>(end - start).count();
		// Join the partial results
#ifdef VERIFY
			bool pass = verify_range_query(c, rangequeries, i, result);
			if (pass == 0)
				cout << "Query : " << i << " " << pass << "\n";
#endif
			fprintf(stderr, "%ld \n", result );
		totalTime.at(i) = scanTime.at(i) + indexCreation.at(i) + indexLookup.at(i) + joinTime.at(i);
	}
	//    Print(*T);
	for (size_t l = 0; l < NUMBER_OF_COLUMNS; ++l)
	{
		free(crackercolumns[l]);
	}
	free(crackercolumns);
}

void *fullIndex(IndexEntry *c)
{
	hybrid_radixsort_insert(c, COLUMN_SIZE);
	void *I = build_bptree_bulk(c, COLUMN_SIZE);

	return I;
}

void bptree_bulk_index3()
{
	chrono::time_point<chrono::system_clock> start, end;
	Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
	loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

	RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
	loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

	start = chrono::system_clock::now();
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
	end = chrono::system_clock::now();
	indexCreation.at(0) += chrono::duration<double>(end - start).count();

	for (size_t i = 0; i < NUM_QUERIES; i++)
	{
		vector<boost::dynamic_bitset<>> bitmaps(NUMBER_OF_COLUMNS);
		// query
		for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
		{
			start = chrono::system_clock::now();
			bitmaps.at(j).resize(COLUMN_SIZE);
			int64_t offset1 = (T[j])->gte(rangequeries[j].leftpredicate[i]);
			int64_t offset2 = (T[j])->lt(rangequeries[j].rightpredicate[i]);
			end = chrono::system_clock::now();
			indexLookup.at(i) += chrono::duration<double>(end - start).count();
			start = chrono::system_clock::now();
			scanQuery(crackercolumns[j], offset1, offset2, bitmaps[j]);
			end = chrono::system_clock::now();
			scanTime.at(i) += chrono::duration<double>(end - start).count();
		}
		int64_t result;
		start = std::chrono::system_clock::now();
		result = join_bitmaps(bitmaps, c);
		end = std::chrono::system_clock::now();
		joinTime.at(i) += std::chrono::duration<double>(end - start).count();
		// Join the partial results
#ifdef VERIFY
			bool pass = verify_range_query(c, rangequeries, i, result);
			if (pass == 0)
				cout << "Query : " << i << " " << pass << "\n";
#endif
			fprintf(stderr, "%ld \n", result );
		totalTime.at(i) = scanTime.at(i) + indexCreation.at(i) + indexLookup.at(i) + joinTime.at(i);
	}
	for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
	{
		free(crackercolumns[i]);
		free(T[i]);
	}
	free(crackercolumns);
	free(T);
}

void kdtree_cracking()
{
	configKDTree(KDTREE_THRESHOLD);
	chrono::time_point<chrono::system_clock> start, end;
	Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
	loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

	RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
	loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

	start = chrono::system_clock::now();
	Table table;
	table.columns = vector<vector<ElementType>>(NUMBER_OF_COLUMNS);
	table.ids = vector<int64_t>(COLUMN_SIZE);
	for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col)
	{
		table.columns.at(col) = vector<ElementType>(COLUMN_SIZE);
		for (size_t line = 0; line < COLUMN_SIZE; ++line)
		{
			table.ids.at(line) = line;
			table.columns.at(col).at(line) = c[col].data[line];
		}
	}

	KDTree index = NULL;
	end = chrono::system_clock::now();
	indexCreation.at(0) += chrono::duration<double>(end - start).count();

	for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index)
	{
		// Transform query in a format easier to handle
		vector<pair<int64_t, int64_t>> query(NUMBER_OF_COLUMNS);
		for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
		{
			query.at(i).first = rangequeries[i].leftpredicate[query_index];
			query.at(i).second = rangequeries[i].rightpredicate[query_index];
		}

		int64_t result = SearchKDTree(index, query, table, true, query_index);
		fprintf(stderr, "%ld \n", result );

#ifdef VERIFY
		bool pass = verify_range_query(c, rangequeries, query_index, result);
		if (pass == 0)
			cout << "Query : " << query_index << " " << pass << "\n";
#endif
		totalTime.at(query_index) = scanTime.at(query_index) + indexCreation.at(query_index) + indexLookup.at(query_index) + joinTime.at(query_index);
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

void full_kdtree()
{
	configKDTree(KDTREE_THRESHOLD);
	chrono::time_point<chrono::system_clock> start, end;
	Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
	loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

	RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
	loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);

	start = chrono::system_clock::now();
	Table table;
	table.columns = vector<vector<ElementType>>(NUMBER_OF_COLUMNS);
	table.ids = vector<int64_t>(COLUMN_SIZE);
	for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col)
	{
		table.columns.at(col) = vector<ElementType>(COLUMN_SIZE);
		for (size_t line = 0; line < COLUMN_SIZE; ++line)
		{
			table.ids.at(line) = line;
			table.columns.at(col).at(line) = c[col].data[line];
		}
	}

	KDTree index = FullKDTree(table);
	end = chrono::system_clock::now();

	indexCreation.at(0) += chrono::duration<double>(end - start).count();

	for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index)
	{
		// Transform query in a format easier to handle
		vector<pair<int64_t, int64_t>> query(NUMBER_OF_COLUMNS);
		for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
		{
			query.at(i).first = rangequeries[i].leftpredicate[query_index];
			query.at(i).second = rangequeries[i].rightpredicate[query_index];
		}

		int64_t result = SearchKDTree(index, query, table, false, query_index);
		fprintf(stderr, "%ld \n", result );

#ifdef VERIFY
		bool pass = verify_range_query(c, rangequeries, query_index, result);
		if (pass == 0)
			cout << "Query : " << query_index << " " << pass << "\n";
#endif
		totalTime.at(query_index) = scanTime.at(query_index) + indexCreation.at(query_index) + indexLookup.at(query_index) + joinTime.at(query_index);
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



void benchmarkFunction(Column *column, vector<vector<pair<int64_t,int64_t>>> rangeQueries, 
	// pre_processing_function pre_processing, partial_index_built_function partial_index_built,
	// index_lookup_function index_lookup, 
	scan_data_function scan_data
	// , intersect_data_function intersect_data
	){
    chrono::time_point<std::chrono::system_clock> start, end;

	// First we do pre-processing. In the case of full index we fully create the index. In the case of Partial Indexes we copy the elements to a cracker index structure.
    // start = chrono::system_clock::now();
    // pre_processing();
    // end = chrono::system_clock::now();
    indexCreation.at(0)  = chrono::duration<double>(end - start).count();
    for(int i = 0; i < NUM_QUERIES; i++) {
    	// If we are running cracking algorithms we do a partial index creation step
    	// start = chrono::system_clock::now();
    	// partial_index_built()
    	// end = chrono::system_clock::now();
     //    indexCreation.at(q) += chrono::duration<double>(end - start).count();

     //    start = chrono::system_clock::now();
    	// index_lookup()
    	// end = chrono::system_clock::now();
     //    indexLookup.at(q)  = chrono::duration<double>(end - start).count();
      
       // Intersecting data for uni-dimensional indexes
        start = chrono::system_clock::now();
    	scan_data(column,&rangeQueries.at(i));
    	end = chrono::system_clock::now();
        scanTime.at(i)  = chrono::duration<double>(end - start).count();
        
     //    start = chrono::system_clock::now(); 
    	// intersect_data()
    	// end = chrono::system_clock::now();
     //    joinTime.at(q)  = chrono::duration<double>(end - start).count();

        // totalTime.at(q)  = indexCreation.at(0) + indexLookup.at(0) + scanTime.at(0) + joinTime.at(0)

        totalTime.at(i)  = scanTime.at(i);

    }
	for (int q = 0; q < NUM_QUERIES; q++){
		// cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
		fprintf(stderr, "%f\n",totalTime.at(q));
	}
}




void print_help(int argc, char** argv) {
    fprintf(stderr, "Unrecognized command line option.\n");
    fprintf(stderr, "Usage: %s [args]\n", argv[0]);
    fprintf(stderr, "   --column-path\n");
    fprintf(stderr, "   --query-path\n");
    fprintf(stderr, "   --num-queries\n");
    fprintf(stderr, "   --column-size\n");
    fprintf(stderr, "   --column-number\n");
    fprintf(stderr, "   --indexing-type\n");
    fprintf(stderr, "   --bptree-elements-per-node\n");
    fprintf(stderr, "   --kdtree-threshold\n");
}


pair<string,string> split_once(string delimited, char delimiter) {
    auto pos = delimited.find_first_of(delimiter);
    return { delimited.substr(0, pos), delimited.substr(pos+1) };
}

int main(int argc, char **argv)
{
	// Default Values
	COLUMN_FILE_PATH = "column";
	QUERIES_FILE_PATH = "query";
	NUM_QUERIES = 1000;
	COLUMN_SIZE = 10000000;
	NUMBER_OF_COLUMNS = 16;
	INDEXING_TYPE = 0;
	BPTREE_ELEMENTSPERNODE = 16384;
	KDTREE_THRESHOLD = 1000;

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
                COLUMN_FILE_PATH = arg_value;
            } else if (arg_name == "query-path") {
                QUERIES_FILE_PATH = arg_value;
            }  else if (arg_name == "num-queries") {
                NUM_QUERIES = stoi(arg_value.c_str());
            } else if (arg_name == "column-size") {
                COLUMN_SIZE = atoi(arg_value.c_str());
            } else if (arg_name == "column-number") {
                NUMBER_OF_COLUMNS = atoi(arg_value.c_str());
            } else if (arg_name == "indexing-type") {
                INDEXING_TYPE = atoi(arg_value.c_str());
            } else if (arg_name == "bptree-elements-per-node") {
                BPTREE_ELEMENTSPERNODE = atoi(arg_value.c_str());
            } else if (arg_name == "kdtree-threshold") {
                KDTREE_THRESHOLD = atoi(arg_value.c_str());
            }  else {
                print_help(argc, argv);
                exit(EXIT_FAILURE);
            }
        }

	indexCreation = vector<double>(NUM_QUERIES, 0);
	indexLookup = vector<double>(NUM_QUERIES, 0);
	scanTime = vector<double>(NUM_QUERIES, 0);
	joinTime = vector<double>(NUM_QUERIES, 0);
	totalTime = vector<double>(NUM_QUERIES);
   
    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);
	vector< vector<pair<int64_t, int64_t>> > query(NUM_QUERIES, vector<pair<int64_t, int64_t>> (NUMBER_OF_COLUMNS));  
	
	for (size_t q = 0; q < NUM_QUERIES; q ++ ){
		for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i){
			query.at(q).at(i).first = rangequeries[i].leftpredicate[q];
			query.at(q).at(i).second = rangequeries[i].rightpredicate[q];
		}
	}

	//FULL SCAN
	if (INDEXING_TYPE == 0)
		benchmarkFunction(c,query,full_scan);		
	

	//CRACKING W/ AVL
	else if (INDEXING_TYPE == 1)
	{
		standardCracking();
		for (int q = 0; q < NUM_QUERIES; q++)
			cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

	// FULL INDEX B+ Tree
	else if (INDEXING_TYPE == 2)
	{
		bptree_bulk_index3();
		for (int q = 0; q < NUM_QUERIES; q++)
			cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

	//  Cracking W/ KD-Tree
	else if (INDEXING_TYPE == 3)
	{
		kdtree_cracking();
		for (int q = 0; q < NUM_QUERIES; q++)
			cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

	// Full Index KD-TREE
	else if (INDEXING_TYPE == 4)
	{
		full_kdtree();
		for (int q = 0; q < NUM_QUERIES; q++)
			cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

}
