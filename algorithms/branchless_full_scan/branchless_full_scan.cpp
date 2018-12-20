#include "branchless_full_scan.h"
#include <algorithm>

void BranchlessFullScan::pre_processing(
    vector<int64_t> &ids,
    vector<vector<int64_t> > &columns
){
    number_of_columns = columns.size();
    data_size = ids.size();
    this->ids.assign(ids.begin(), ids.end());
    this->columns.resize(number_of_columns);

    for(size_t i = 0; i < number_of_columns; i++)
    {
        this->columns.at(i) = (int64_t*) malloc(sizeof(int64_t)* data_size);

        for(size_t j = 0; j < columns.at(i).size(); j++)
        {
            this->columns.at(i)[j] = columns.at(i).at(j);
        }
    }
}
void BranchlessFullScan::search(
    vector<array<int64_t, 3> > &query
){
    int64_t vector_size = 2000; // 2000*64 = 128000 bits 1/2 L1.
	size_t sel_size;
	int64_t sel_vector [vector_size];
	int64_t count = 0;
	int64_t low, high, col;
	int64_t i = 0;
    resulting_ids.resize(0);
    for (i = 0; i < data_size - vector_size; i+= vector_size){
		low = query.at(0).at(0);
		high = query.at(0).at(1);
		col = query.at(0).at(2);
		sel_size = select_rq_scan_new (sel_vector, &columns.at(col)[i], low, high, vector_size);
		for (size_t query_num = 1; query_num < query.size(); query_num++)
		{
			low = query.at(query_num).at(0);
			high = query.at(query_num).at(1);
			col = query.at(query_num).at(2);
			sel_size = select_rq_scan_sel_vec(sel_vector, &columns.at(col)[i], low, high, sel_size);
		}
		for(size_t j = 0; j < sel_size; ++ j)
			resulting_ids.push_back(ids[i+sel_vector[j]]);
	}

	low = query.at(0).at(0);
	high = query.at(0).at(1);
	col = query.at(0).at(2);
	sel_size = select_rq_scan_new (sel_vector, &columns.at(col)[i], low, high, data_size - i);
	for (size_t query_num = 1; query_num < query.size(); query_num++)
	{
		low = query.at(query_num).at(0);
		high = query.at(query_num).at(1);
		col = query.at(query_num).at(2);
		sel_size = select_rq_scan_sel_vec(sel_vector, &columns.at(col)[i], low, high, sel_size);
	}

	for(size_t j = 0; j < sel_size; ++ j)
		resulting_ids.push_back(ids[i+sel_vector[j]]);
}

vector<int64_t> BranchlessFullScan::get_result(){
    return resulting_ids;
}


int64_t BranchlessFullScan::select_rq_scan_sel_vec(
    int64_t*__restrict__ sel,
    int64_t*__restrict__ col,
    int64_t keyL, int64_t keyH, int64_t n
){
    int64_t j;
    for (int64_t i = j = 0; i < n; i++){
        int matching =  ((keyL <= col[sel[i]]) || (keyL == -1)) &&  ((col[sel[i]] < keyH) || (keyH == -1));
        sel[j] = sel[i];
        j += matching;
    }
    return j;

}

int64_t BranchlessFullScan::select_rq_scan_new (
    int64_t*__restrict__ sel,
    int64_t*__restrict__ col,
    int64_t keyL, int64_t keyH, int64_t n
){
    int64_t j;
    for (int64_t i = j = 0 ; i < n; i++){
        int matching =  ((keyL <= col[i]) || (keyL == -1)) &&  ((col[i] < keyH) || (keyH == -1));

        sel[j] = i;
        j += matching;

    }
    return j;
}