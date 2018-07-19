#include "util.h"
#include <unordered_map>
#include <vector>
#include <array>

#define test

using namespace std;
extern int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS;

void create_bitmap(IndexEntry *c, int64_t from, int64_t to, boost::dynamic_bitset<> &bitmap)
{
	for (size_t i = from; i <= to; i++)
	{
		boost::dynamic_bitset<>::size_type id = (boost::dynamic_bitset<>::size_type) c[i].m_rowId;
		bitmap[id] = 1;
	}
}

int64_t count_bitmap(boost::dynamic_bitset<> bitmap){
	int64_t count =0;
	for(boost::dynamic_bitset<>::size_type i = 0; i < COLUMN_SIZE; ++i){
		if(bitmap[i]){
			count += 1;
		}
	}
	
	return count;
}

vector<int64_t>  result_bitmap(boost::dynamic_bitset<> bitmap){
	vector<int64_t>  result;
	for(boost::dynamic_bitset<>::size_type i = 0; i < COLUMN_SIZE; ++i){
		if(bitmap[i]){
			result.push_back(i);
		}
	}
	
	return result;
}

vector<int64_t> join_bitmaps(vector<boost::dynamic_bitset<>> *bitmaps){
	vector<int64_t> result;
	if(bitmaps->size() > 1){
		boost::dynamic_bitset<> final_bitmap(COLUMN_SIZE);

		for(boost::dynamic_bitset<>::size_type i = 0; i < COLUMN_SIZE; ++i){
			final_bitmap[i] = bitmaps->at(0)[i];
		}

		for(boost::dynamic_bitset<>::size_type i = 1; i < bitmaps->size(); ++i){
			final_bitmap = (final_bitmap & bitmaps->at(i));
		}
		#ifdef test
			result = result_bitmap(final_bitmap);
		#else
			result.push_back(count_bitmap(final_bitmap));
		#endif
		return result;
	}else{
		#ifdef test
			result = result_bitmap(bitmaps->at(0));
		#else
			result.push_back(count_bitmap(bitmaps->at(0)));
		#endif
		return result;
	}

}

int select_rq_scan_sel_vec (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n){
    int j;
    for (int i = j = 0; i < n; i++){
        int matching =  ((keyL <= col[sel[i]]) || (keyL == -1)) &&  ((col[sel[i]] < keyH) || (keyH == -1));
        sel[j] = sel[i];
        j += matching;
    }
    return j;

}

int select_rq_scan_new (int*__restrict__ sel, int64_t*__restrict__ col, int64_t keyL, int64_t keyH, int n){
    int j;
    for (int i = j = 0 ; i < n; i++){
        int matching =  ((keyL <= col[i]) || (keyL == -1)) &&  ((col[i] < keyH) || (keyH == -1));

        sel[j] = i;
        j += matching;

    }
    return j;
}

void full_scan(Table *table, vector<array<int64_t, 3>>  *rangequeries, vector<pair<int,int>> *offsets, vector<int64_t> * result)
{
	size_t vector_size = 2000; // 2000*64 = 128000 bits 1/2 L1.
	size_t sel_size;
	int sel_vector [vector_size];
	int64_t count = 0;
	int64_t low, high, col;
	for (size_t i = 0; i < COLUMN_SIZE/vector_size; ++ i){
		low = rangequeries->at(0).at(0);
		high = rangequeries->at(0).at(1);
		col = rangequeries->at(0).at(2);
		sel_size = select_rq_scan_new (sel_vector, &table->columns[col][vector_size*i], low, high, vector_size);
		for (size_t query_num = 1; query_num < rangequeries->size(); query_num++)
		{
			low = rangequeries->at(query_num).at(0);
			high = rangequeries->at(query_num).at(1);
			col = rangequeries->at(query_num).at(2);
			sel_size = select_rq_scan_sel_vec(sel_vector, &table->columns[col][vector_size*i], low, high, sel_size);
		}
		#ifdef test
			for(size_t j = 0; j < sel_size; ++ j)
				result->push_back(vector_size*i+sel_vector[j]);
		#else
			count += sel_size;
		#endif
	}
	#ifndef test
		result->push_back(count);
	#endif
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
    fprintf(stderr, "   --kdtree-threshold\n");
}


pair<string,string> split_once(string delimited, char delimiter) {
    auto pos = delimited.find_first_of(delimiter);
    return { delimited.substr(0, pos), delimited.substr(pos+1) };
}