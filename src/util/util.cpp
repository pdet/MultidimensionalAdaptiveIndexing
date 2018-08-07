#include "util.h"
#include <unordered_map>
#include <vector>
#include <array>

// #define test

using namespace std;
extern int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS;

void create_bitmap(IndexEntry *c, int64_t from, int64_t to, vector<bool> &bitmap)
{
	for (int64_t i = from; i <= to; i++)
	{
		size_t id = (size_t) c[i].m_rowId;
		bitmap[id] = 1;
	}
}

int64_t count_bitmap(vector<bool> bitmap){
	int64_t count =0;
	for(size_t i = 0; i < COLUMN_SIZE; ++i){
		if(bitmap[i]){
			count += 1;
		}
	}
	
	return count;
}

vector<int64_t>  result_bitmap(vector<bool> bitmap){
	vector<int64_t>  result;
	for(size_t i = 0; i < COLUMN_SIZE; ++i){
		if(bitmap[i]){
			result.push_back(i);
		}
	}
	
	return result;
}

vector<int64_t> join_bitmaps(vector<vector<bool> > *bitmaps){
	vector<int64_t> result;
	if(bitmaps->size() > 1){
		vector<bool> final_bitmap(COLUMN_SIZE);

		for(size_t i = 0; i < COLUMN_SIZE; ++i){
			final_bitmap[i] = bitmaps->at(0)[i];
		}

		for(size_t i = 1; i < bitmaps->size(); ++i){
			for(size_t j = 0; j < COLUMN_SIZE; j++)
			{
				final_bitmap.at(j) = (final_bitmap.at(j) & bitmaps->at(i).at(j));
			}
		}
		result = result_bitmap(final_bitmap);
		return result;
	}else{
		result = result_bitmap(bitmaps->at(0));
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
	ino64_t vector_size = 2000; // 2000*64 = 128000 bits 1/2 L1.
	size_t sel_size;
	int sel_vector [vector_size];
	int64_t count = 0;
	int64_t low, high, col;
	int64_t i = 0;
	for (i = 0; i < COLUMN_SIZE - vector_size; i+= vector_size){
		low = rangequeries->at(0).at(0);
		high = rangequeries->at(0).at(1);
		col = rangequeries->at(0).at(2);
		sel_size = select_rq_scan_new (sel_vector, &table->columns[col][i], low, high, vector_size);
		for (size_t query_num = 1; query_num < rangequeries->size(); query_num++)
		{
			low = rangequeries->at(query_num).at(0);
			high = rangequeries->at(query_num).at(1);
			col = rangequeries->at(query_num).at(2);
			sel_size = select_rq_scan_sel_vec(sel_vector, &table->columns[col][i], low, high, sel_size);
		}
		for(size_t j = 0; j < sel_size; ++ j)
			result->push_back(table->ids[i+sel_vector[j]]);
	}

	low = rangequeries->at(0).at(0);
	high = rangequeries->at(0).at(1);
	col = rangequeries->at(0).at(2);
	sel_size = select_rq_scan_new (sel_vector, &table->columns[col][i], low, high, COLUMN_SIZE - i);
	for (size_t query_num = 1; query_num < rangequeries->size(); query_num++)
	{
		low = rangequeries->at(query_num).at(0);
		high = rangequeries->at(query_num).at(1);
		col = rangequeries->at(query_num).at(2);
		sel_size = select_rq_scan_sel_vec(sel_vector, &table->columns[col][i], low, high, sel_size);
	}

	for(size_t j = 0; j < sel_size; ++ j)
		result->push_back(table->ids[i+sel_vector[j]]);
}

int64_t uniformRandomProjection(vector<int64_t> * ids){
	return ids->size();
}

int64_t tpchProjection(Table * T,vector<int64_t> * ids) {
	int64_t result = 0;
	for(size_t i = 0; i < ids->size(); ++i){
		int64_t id = ids->at(i);
		result += T->columns.at(5).at(id) * T->columns.at(6).at(id);
	}
	return result;
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