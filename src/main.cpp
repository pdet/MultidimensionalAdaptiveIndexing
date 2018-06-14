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

// #define PREDICATED

std::string COLUMN_FILE_PATH, QUERIES_FILE_PATH;
extern int64_t COLUMN_SIZE, BPTREE_ELEMENTSPERNODE;
int64_t NUM_QUERIES, NUMBER_OF_COLUMNS, KDTREE_THRESHOLD;

std::vector<double> indexCreation;
std::vector<double> indexLookup;
std::vector<double> scanTime;
std::vector<double> joinTime;
std::vector<double> totalTime;

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

		std::set<int64_t> result;
		std::set_difference(r1.begin(), r1.end(), received.begin(), received.end(),
							std::inserter(result, result.end()));
		fprintf(stderr, "Difference:\n");
		for (it = result.begin(); it != result.end(); ++it)
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

void scanQuery(IndexEntry *c, int64_t from, int64_t to, boost::dynamic_bitset<> &bitmap)
{
	for (size_t i = from; i <= to; i++)
	{
		boost::dynamic_bitset<>::size_type id = (boost::dynamic_bitset<>::size_type) c[i].m_rowId;
		bitmap[id] = 1;
	}
}

std::set<int64_t> bitmap_to_set(boost::dynamic_bitset<> bitmap, IndexEntry ** columns){
	std::set<int64_t> result;
	for(boost::dynamic_bitset<>::size_type i = 0; i < COLUMN_SIZE; ++i){
		if(bitmap[i]){
			result.insert(i);
		}
	}

	return result;
}

std::set<int64_t>
join_bitmaps(std::vector<boost::dynamic_bitset<>> bitmaps, IndexEntry ** columns){
	boost::dynamic_bitset<> final_bitmap(COLUMN_SIZE);

	for(boost::dynamic_bitset<>::size_type i = 0; i < COLUMN_SIZE; ++i){
		final_bitmap[i] = bitmaps.at(0)[i];
	}

	for(boost::dynamic_bitset<>::size_type i = 1; i < bitmaps.size(); ++i){
		final_bitmap = (final_bitmap & bitmaps.at(i));
	}

	return bitmap_to_set(final_bitmap, columns);

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


std::set<int64_t> tuple_scan(IndexEntry **c, RangeQuery *queries, size_t query_index, int64_t from, int64_t to)
{
	std::set<int64_t> ids;
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
	//Initialitizing multiple Cracker Indexes
	AvlTree *T = (AvlTree *)malloc(sizeof(AvlTree) * NUMBER_OF_COLUMNS);
	for (size_t k = 0; k < NUMBER_OF_COLUMNS; ++k)
	{
		T[k] = NULL;
	}
	end = std::chrono::system_clock::now();
	indexCreation.at(0) += std::chrono::duration<double>(end - start).count();

	for (size_t i = 0; i < NUM_QUERIES; i++)
	{
		std::vector<boost::dynamic_bitset<>> bitmaps(NUMBER_OF_COLUMNS);
		for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
		{
			//Partitioning Column and Inserting in Cracker Indexing
			start = std::chrono::system_clock::now();
			bitmaps.at(j).resize(COLUMN_SIZE);
			T[j] = standardCracking(
				crackercolumns[j],
				COLUMN_SIZE,
				T[j],
				rangequeries[j].leftpredicate[i],
				rangequeries[j].rightpredicate[i]);
			end = std::chrono::system_clock::now();
			indexCreation.at(i) += std::chrono::duration<double>(end - start).count();
			start = std::chrono::system_clock::now();
			//Querying
			IntPair p1 = FindNeighborsGTE(rangequeries[j].leftpredicate[i], T[j], COLUMN_SIZE - 1);
			IntPair p2 = FindNeighborsLT(rangequeries[j].rightpredicate[i], T[j], COLUMN_SIZE - 1);
			int offset1 = p1->first;
			int offset2 = p2->second;
			free(p1);
			free(p2);
			end = std::chrono::system_clock::now();
			indexLookup.at(i) += std::chrono::duration<double>(end - start).count();
			start = std::chrono::system_clock::now();
			scanQuery(crackercolumns[j], offset1, offset2, bitmaps[j]);
			end = std::chrono::system_clock::now();
			scanTime.at(i) += std::chrono::duration<double>(end - start).count();
		}
		std::set<int64_t> result;
		// Join the partial results
		if (NUMBER_OF_COLUMNS == 1)
		{

#ifdef VERIFY
			bool pass = verify_range_query(c, rangequeries, i, join_bitmaps(bitmaps, crackercolumns));
			if (pass == 0)
				std::cout << "Query : " << i << " " << pass << "\n";
#endif
		}
		else
		{
			start = std::chrono::system_clock::now();
			result = join_bitmaps(bitmaps, crackercolumns);
			end = std::chrono::system_clock::now();
			joinTime.at(i) += std::chrono::duration<double>(end - start).count();
#ifdef VERIFY
			bool pass = verify_range_query(c, rangequeries, i, result);
			if (pass == 0)
				std::cout << "Query : " << i << " " << pass << "\n";
#endif
		}
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
	indexCreation.at(0) += std::chrono::duration<double>(end - start).count();

	for (size_t i = 0; i < NUM_QUERIES; i++)
	{
		std::vector<boost::dynamic_bitset<>> bitmaps(NUMBER_OF_COLUMNS);
		// query
		for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
		{
			start = std::chrono::system_clock::now();
			bitmaps.at(j).resize(COLUMN_SIZE);
			int64_t offset1 = (T[j])->gte(rangequeries[j].leftpredicate[i]);
			int64_t offset2 = (T[j])->lt(rangequeries[j].rightpredicate[i]);
			end = std::chrono::system_clock::now();
			indexLookup.at(i) += std::chrono::duration<double>(end - start).count();
			start = std::chrono::system_clock::now();
			scanQuery(crackercolumns[j], offset1, offset2, bitmaps[j]);
			end = std::chrono::system_clock::now();
			scanTime.at(i) += std::chrono::duration<double>(end - start).count();
		}

		std::set<int64_t> result;
		// Join the partial results
		if (NUMBER_OF_COLUMNS == 1)
		{
#ifdef VERIFY
			bool pass = verify_range_query(c, rangequeries, i, join_bitmaps(bitmaps, crackercolumns));
			if (pass == 0)
				std::cout << "Query : " << i << " " << pass << "\n";
#endif
		}
		else
		{
			start = std::chrono::system_clock::now();
			result = join_bitmaps(bitmaps, crackercolumns);
			end = std::chrono::system_clock::now();
			joinTime.at(i) += std::chrono::duration<double>(end - start).count();

#ifdef VERIFY
			bool pass = verify_range_query(c, rangequeries, i, result);
			if (pass == 0)
				std::cout << "Query : " << i << " " << pass << "\n";
#endif
		}
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
	indexCreation.at(0) += std::chrono::duration<double>(end - start).count();

	for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index)
	{
		// Transform query in a format easier to handle
		std::vector<std::pair<int64_t, int64_t>> query(NUMBER_OF_COLUMNS);
		for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
		{
			query.at(i).first = rangequeries[i].leftpredicate[query_index];
			query.at(i).second = rangequeries[i].rightpredicate[query_index];
		}

		std::vector<int64_t> result = SearchKDTree(index, query, table, true, query_index);

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

	indexCreation.at(0) += std::chrono::duration<double>(end - start).count();

	for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index)
	{
		// Transform query in a format easier to handle
		std::vector<std::pair<int64_t, int64_t>> query(NUMBER_OF_COLUMNS);
		for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
		{
			query.at(i).first = rangequeries[i].leftpredicate[query_index];
			query.at(i).second = rangequeries[i].rightpredicate[query_index];
		}

		std::vector<int64_t> result = SearchKDTree(index, query, table, false, query_index);

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


void full_scan()
{
    std::chrono::time_point<std::chrono::system_clock> start, end;

    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
    loadcolumn(c, COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);

    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
    loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);
    std::vector<int64_t> result;
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

    for (size_t q = 0; q < NUM_QUERIES; q++)
    {
        start = std::chrono::system_clock::now();
		ResultStruct results;
	    results.reserve(NUM_QUERIES);
	    results.values[q] = 0;
        for(size_t i = 0; i < COLUMN_SIZE; ++i){
 			#ifdef PREDICATED
        	// Only works for 16 columns for now. Adding Jit-compilation to make it work with others in near future.
        	int matching = crackercolumns[0][i].m_key >= rangequeries[0].leftpredicate[q] && crackercolumns[0][i].m_key <= rangequeries[0].rightpredicate[q] &&
        	crackercolumns[1][i].m_key >= rangequeries[1].leftpredicate[q] && crackercolumns[1][i].m_key <=rangequeries[1].rightpredicate[q] &&
        	crackercolumns[2][i].m_key >= rangequeries[2].leftpredicate[q] && crackercolumns[2][i].m_key <= rangequeries[2].rightpredicate[q] &&
        	crackercolumns[3][i].m_key >= rangequeries[3].leftpredicate[q] && crackercolumns[3][i].m_key <= rangequeries[3].rightpredicate[q] &&
        	crackercolumns[4][i].m_key >= rangequeries[4].leftpredicate[q] && crackercolumns[4][i].m_key <= rangequeries[4].rightpredicate[q] &&
        	crackercolumns[5][i].m_key >= rangequeries[5].leftpredicate[q] && crackercolumns[5][i].m_key <= rangequeries[5].rightpredicate[q] &&
        	crackercolumns[6][i].m_key >= rangequeries[6].leftpredicate[q] && crackercolumns[6][i].m_key <= rangequeries[6].rightpredicate[q] &&
        	crackercolumns[7][i].m_key >= rangequeries[7].leftpredicate[q] && crackercolumns[7][i].m_key <= rangequeries[7].rightpredicate[q] &&
        	crackercolumns[8][i].m_key >= rangequeries[8].leftpredicate[q] && crackercolumns[8][i].m_key <= rangequeries[8].rightpredicate[q] &&
        	crackercolumns[9][i].m_key >= rangequeries[9].leftpredicate[q] && crackercolumns[9][i].m_key <= rangequeries[9].rightpredicate[q] &&
        	crackercolumns[10][i].m_key >= rangequeries[10].leftpredicate[q] && crackercolumns[10][i].m_key <= rangequeries[10].rightpredicate[q] &&
        	crackercolumns[11][i].m_key >= rangequeries[11].leftpredicate[q] && crackercolumns[11][i].m_key <= rangequeries[11].rightpredicate[q] &&
        	crackercolumns[12][i].m_key >= rangequeries[12].leftpredicate[q] && crackercolumns[12][i].m_key <= rangequeries[12].rightpredicate[q] &&
        	crackercolumns[13][i].m_key >= rangequeries[13].leftpredicate[q] && crackercolumns[13][i].m_key <= rangequeries[13].rightpredicate[q] &&
        	crackercolumns[14][i].m_key >= rangequeries[14].leftpredicate[q] && crackercolumns[14][i].m_key <= rangequeries[14].rightpredicate[q] &&
        	crackercolumns[15][i].m_key >= rangequeries[15].leftpredicate[q] && crackercolumns[15][i].m_key <= rangequeries[15].rightpredicate[q];
        	results.values[q] = results.values[q] + matching;
        	#else
            bool match = true;
            for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
            {
                if (crackercolumns[j][i].m_key < rangequeries[j].leftpredicate[q] || crackercolumns[j][i].m_key > rangequeries[j].rightpredicate[q]){
                    match = false;
                    break;
                }
            }
            if (match)
                result.push_back(i);
        	#endif

        }
        end = std::chrono::system_clock::now();
        scanTime.at(q) = std::chrono::duration<double>(end - start).count();
        // Join the partial results
        totalTime.at(q) = scanTime.at(q) + indexCreation.at(q) + indexLookup.at(q) + joinTime.at(q);
    }
    for (size_t i = 0; i < NUMBER_OF_COLUMNS; ++i)
    {
        free(crackercolumns[i]);
    }
    free(crackercolumns);
}


//.column.txt .query.txt 10 10000 0 2 16384
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

	indexCreation = std::vector<double>(NUM_QUERIES, 0);
	indexLookup = std::vector<double>(NUM_QUERIES, 0);
	scanTime = std::vector<double>(NUM_QUERIES, 0);
	joinTime = std::vector<double>(NUM_QUERIES, 0);
	totalTime = std::vector<double>(NUM_QUERIES);

	//FULL SCAN
	if (INDEXING_TYPE == 0)
	{
		full_scan();
		for (int q = 0; q < NUM_QUERIES; q++){
			std::cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
			fprintf(stderr, "%f\n",totalTime.at(q));
		}
			
	}

	//CRACKING W/ AVL
	else if (INDEXING_TYPE == 1)
	{
		standardCracking();
		for (int q = 0; q < NUM_QUERIES; q++)
			std::cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

	// FULL INDEX B+ Tree
	else if (INDEXING_TYPE == 2)
	{
		BPTREE_ELEMENTSPERNODE = atoi(argv[7]);
		bptree_bulk_index3();
		for (int q = 0; q < NUM_QUERIES; q++)
			std::cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

	//  Cracking W/ KD-Tree
	else if (INDEXING_TYPE == 3)
	{
		KDTREE_THRESHOLD = atoi(argv[7]);
		kdtree_cracking();
		for (int q = 0; q < NUM_QUERIES; q++)
			std::cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

	// Full Index KD-TREE
	else if (INDEXING_TYPE == 4)
	{
		KDTREE_THRESHOLD = atoi(argv[7]);
		full_kdtree();
		for (int q = 0; q < NUM_QUERIES; q++)
			std::cout << indexCreation.at(q) << ";" << indexLookup.at(q) << ";" << scanTime.at(q) << ";" << joinTime.at(q) << ";" << totalTime.at(q) << "\n";
	}

}
