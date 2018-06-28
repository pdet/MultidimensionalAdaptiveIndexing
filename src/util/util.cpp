#include "util.h"
#include <unordered_map>
#include <vector>

using namespace std;
extern int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS;

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
			result += 1;
		}
	}

	return result;
}

int64_t join_bitmaps(std::vector<boost::dynamic_bitset<>> bitmaps, Column * columns){
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
    for (int i = j = 0 ; i < n; i++){
        int matching =  keyL <= col[i] &&  col[i] < keyH;

        sel[j] = i;
        j += matching;

    }
    return j;
}

int full_scan(Column *c, vector<pair<int64_t,int64_t>>  *rangequeries)
{
	size_t vector_size = 2000; // 2000*64 = 128000 bits 1/2 L1.
	size_t sel_size;
	int sel_vector [vector_size];
	int res;
	for (size_t i = 0; i < COLUMN_SIZE/vector_size; ++ i){
		sel_size = select_rq_scan_new (sel_vector, &c[0].data[vector_size*i],rangequeries->at(0).first,rangequeries->at(0).second,vector_size);
		for (size_t column_num = 1; column_num < NUMBER_OF_COLUMNS; column_num++){
			sel_size = select_rq_scan_sel_vec(sel_vector, &c[column_num].data[vector_size*i],rangequeries->at(column_num).first,rangequeries->at(column_num).second,sel_size);
		}
		res += sel_size;
	}
	return res;
}