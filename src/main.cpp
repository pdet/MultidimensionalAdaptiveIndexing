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
#include <boost/dynamic_bitset.hpp>

#include "cracking/avl_tree.h"

#include "cracking/kd_tree.h"
#include "cracking/standard_cracking.h"
#include "fullindex/bulkloading_bp_tree.h"
#include "fullindex/hybrid_radix_insert_sort.h"
#include "util/file_manager.h"
#include "util/structs.h"

#include "util/timer.h"


#define VERIFY

using namespace std;


string COLUMN_FILE_PATH, QUERIES_FILE_PATH;
extern int64_t COLUMN_SIZE, BPTREE_ELEMENTSPERNODE;
int64_t NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD, INDEXING_TYPE;

vector<double> indexCreation;
vector<double> indexLookup;
vector<double> scanTime;
vector<double> joinTime;
vector<double> totalTime;

int64_t range_query_baseline(Column *c, RangeQuery *queries, size_t query_index)
{
	int64_t result = 0;
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
			result += c[0].data[i];
		}
	}
	return result;
}

bool verify_range_query(Column *c, RangeQuery *queries, size_t query_index, int64_t received)
{
	int64_t result = range_query_baseline(c, queries, query_index);
	if (received != result)
	{
		fprintf(stderr, "Incorrect Results!\n");
		fprintf(stderr, "Expected: %ld\n", result);
		fprintf(stderr, "Got: %ld\n", received);
		assert(0);
		return false;
	}
	return true;
}

void scanQuery(IndexEntry *c, int64_t from, int64_t to, boost::dynamic_bitset<> &bitmap)
{
	for (size_t i = from; i <= to; i++)
	{
		boost::dynamic_bitset<>::size_type id = (boost::dynamic_bitset<>::size_type) c[i].m_rowId;
		bitmap[id] = 1;
	}
}

int64_t sum_bitmap(boost::dynamic_bitset<> bitmap, Column first_column){
	int64_t result = 0;
	size_t j = 0;
	for(boost::dynamic_bitset<>::size_type i = 0; i < COLUMN_SIZE; ++i, ++j){
		if(bitmap[i]){
			result += first_column.data[j];
		}
	}

	return result;
}

int64_t
join_bitmaps(std::vector<boost::dynamic_bitset<>> bitmaps, Column * columns){
	if(bitmaps.size() > 1){
		boost::dynamic_bitset<> final_bitmap(COLUMN_SIZE);

		for(boost::dynamic_bitset<>::size_type i = 0; i < COLUMN_SIZE; ++i){
			final_bitmap[i] = bitmaps.at(0)[i];
		}

		for(boost::dynamic_bitset<>::size_type i = 1; i < bitmaps.size(); ++i){
			final_bitmap = (final_bitmap & bitmaps.at(i));
		}

		return sum_bitmap(final_bitmap, columns[0]);
	}else{
		return sum_bitmap(bitmaps.at(0), columns[0]);
	}

}

void filterQuery3(IndexEntry *c, RangeQuery queries, size_t query_index, int64_t from, int64_t to,
				  boost::dynamic_bitset<> &results, bool first_time)
{
	int64_t keyL = queries.leftpredicate[query_index];
	int64_t keyH = queries.rightpredicate[query_index];
	boost::dynamic_bitset<>::size_type j = from;
	// fprintf(stderr, "----------------------");
	if(first_time){
		for (size_t i = from; i <= to; ++i, ++j)
		{
			// fprintf(stderr, "KeyL: %ld, key: %ld, keyH: %ld\n", keyL, c[i].m_key, keyH);
			if (c[i].m_key >= keyL && c[i].m_key < keyH)
			{
				results[j] = 1;
			}
		}
	}else{
		for (size_t i = from; i <= to; ++i, ++j)
		{
			if(results[j]) {
				if (!(c[i].m_key >= keyL && c[i].m_key < keyH))
				{
					results[j] = 0;
				}
			}
		}
	}
}


set<int64_t> tuple_scan(IndexEntry **c, RangeQuery *queries, size_t query_index, int64_t from, int64_t to)
{
	set<int64_t> ids;
	for (size_t i = from; i <= to; ++i)
	{
		bool is_valid = true;
		for (size_t j = 0; j < NUMBER_OF_COLUMNS && is_valid; ++j)
		{
			int64_t keyL = queries[j].leftpredicate[query_index];
			int64_t keyH = queries[j].rightpredicate[query_index];
			if (!(c[j][i].m_key >= keyL && c[j][i].m_key < keyH))
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

#ifdef VERIFY
		bool pass = verify_range_query(c, rangequeries, query_index, result);
		if (pass == 0)
			cout << "Query : " << query_index << " " << pass << "\n";
#endif
		totalTime.at(query_index) = scanTime.at(query_index) + indexCreation.at(query_index) + indexLookup.at(query_index) + joinTime.at(query_index);
	}
	//    Print(index);
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

struct ResultStruct {
	int* values = nullptr;
	size_t elements = 0;

	void reserve(size_t capacity) {
		values = new int[capacity];
	}

	size_t size() { return elements; }
	int* begin() { return values; }
	int* end() { return values + elements; }
	inline void push_back(int value) {
		values[elements++] = value;
	}
	int& operator[] (const int index) {
		return values[index];
	}
	const int operator[] (const int index) const {
		return values[index];
	}

	ResultStruct() : values(nullptr), elements(0) { }
};

int select_rq_scan_sel_vec (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n){
	int j;
	for (int i = j = 0; i < n; i++){
		int matching =  keyL <= col[sel[i]] &&  col[sel[i]] < keyH;
		sel[j] = sel[i];
		j += matching;
	}
	return j;

}

int select_rq_scan_new (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n){
	int j;
	// fprintf(stderr, "%d", n);
	for (int i = j = 0 ; i < n; i++){
		int matching =  keyL <= col[i] &&  col[i] < keyH;

		sel[j] = i;
		j += matching;

	}
	return j;
}

void full_scan()
{
	size_t vector_size = 2000; // 2000*64 = 128000 bits 1/2 L1.
    chrono::time_point<chrono::system_clock> start, end;

    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);
	int sel_size;
	int sel_vector [vector_size];
    int res [NUM_QUERIES];
    for (int q = 0; q < NUM_QUERIES; q ++){
    	res[q] = 0;

    	start = chrono::system_clock::now();
    	for (size_t i = 0; i < COLUMN_SIZE/vector_size; ++ i){
			sel_size = select_rq_scan_new (sel_vector, &c[0].data[vector_size*i],rangequeries[0].leftpredicate[q],rangequeries[0].rightpredicate[q],vector_size);
			for (int column_num = 1; column_num < NUMBER_OF_COLUMNS; column_num++){
				sel_size = select_rq_scan_sel_vec(sel_vector, &c[column_num].data[vector_size*i],rangequeries[column_num].leftpredicate[q],rangequeries[column_num].rightpredicate[q],sel_size);
			}
    		res[q] += sel_size;
    	}
        end = chrono::system_clock::now();
        totalTime.at(q)  = chrono::duration<double>(end - start).count();
    	fprintf(stderr, "%d \n", res[q] );
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

	//FULL SCAN
	if (INDEXING_TYPE == 0)
	{
		full_scan();
		for (int q = 0; q < NUM_QUERIES; q++){
			cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
			fprintf(stderr, "%f\n",totalTime.at(q));
		}
			
	}

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
