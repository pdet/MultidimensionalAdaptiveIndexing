#include "quasii.h"

using namespace std;

extern int64_t NUMBER_OF_COLUMNS,COLUMN_SIZE;


const int last_level_threshold = 60; // No clue how to set up this parameter for highly dimensional queries.
vector<int64_t> dimension_threshold;

// Caculate threshold of each level
void calculate_level_thresholds(){
	dimension_threshold.push_back(last_level_threshold);
	double root_aux = 1/NUMBER_OF_COLUMNS;
	int64_t r = ceil(pow(COLUMN_SIZE/last_level_threshold, (double) 1/NUMBER_OF_COLUMNS));
	

	int64_t cur_thr = r * last_level_threshold;
	dimension_threshold.push_back(cur_thr);
	for (int i = 2; i  < NUMBER_OF_COLUMNS; i ++){
		cur_thr = r * cur_thr;
		dimension_threshold.push_back(cur_thr);
	}
	reverse(dimension_threshold.begin(),dimension_threshold.end());

	for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
		fprintf(stderr, "%lld\n",dimension_threshold[i] );
}
struct less_than_offset
{
    inline bool operator() (const Slice& s1, const Slice& s2)
    {
        return (s1.data_offset_begin < s2.data_offset_begin);
    }
};

int binarySearch(vector<Slice> *S, int64_t key){
	int pos = -1;
	int64_t  min = 0;
	int64_t  max = S->size();

	while (max >= min && pos == -1) {
		int mid = (max+min)/2;

		if(S->at(mid).box_begin ==  key){
			pos = mid;
		}else if(S->at(mid).box_begin < key){
			min = mid +1;
		}else if(S->at(mid).box_begin > key){
			max = mid -1;
		}

	}
	return pos;
}

// Same as Cracking-in-Three
vector<Slice> sliceThreeWay(Slice S, Table *table, vector<array<int64_t, 3>>  *rangequeries){
	vector<Slice> result;
	return result;
}
// Same as Cracking-in-Two
vector<Slice> sliceTwoWay(Slice S, Table *table, vector<array<int64_t, 3>>  *rangequeries){
	vector<Slice> result;
	return result;
}

// Cracking in two on key X = (lowkey+highkey)/2
vector<Slice> sliceArtificial(Slice S, Table *table, vector<array<int64_t, 3>>  *rangequeries){
	vector<Slice> result;
	return result;
}

vector<Slice> refine(Slice S, Table *table, vector<array<int64_t, 3>>  *rangequeries){
    int64_t low = rangequeries->at(S.level).at(0);
    int64_t high = rangequeries->at(S.level).at(1);
	vector<Slice> result_slices;
	vector<Slice> refined_slice;
	if ((S.data_offset_end - S.data_offset_begin) <= dimension_threshold[S.level]){
		refined_slice.push_back(S);
		return refined_slice;
	}

 	if (S.box_begin <= low && S.box_end >= high ) // lower and high are within box
 		refined_slice = sliceThreeWay(S, table, rangequeries);
 	else if (S.box_begin <= low && S.box_end < high )
 		refined_slice = sliceTwoWay(S, table, rangequeries);
 	else if (S.box_begin > low && S.box_end >= high )
 		refined_slice = sliceTwoWay(S, table, rangequeries);
 	else
 		refined_slice = sliceArtificial(S, table, rangequeries);
 	for (size_t i = 0; i < refined_slice.size(); i ++){
	    low = rangequeries->at(S.level).at(0);
    	high = rangequeries->at(S.level).at(1);
 		if((refined_slice[i].data_offset_end - refined_slice[i].data_offset_begin)>dimension_threshold[refined_slice[i].level] && 
 			((refined_slice[i].box_begin < low && refined_slice[i].box_end < low) || (refined_slice[i].box_begin > high && refined_slice[i].box_end > high))){
 			vector<Slice> refined_slice_aux = sliceArtificial(refined_slice[i], table, rangequeries);
			result_slices.insert(result_slices.end(), refined_slice_aux.begin(), refined_slice_aux.end());
 		}
 		else{
 			result_slices.push_back(refined_slice[i]);
 		}
 	}
 	return result_slices;

}
// Sort the slices based on the lower offset
//void createDefaultChild(Slice *refined_slice_aux){
//	refined_slice_aux->refined_slices
//
//}

void query(vector<array<int64_t, 3>>  *rangequeries, Table *table, vector<Slice> *S, vector<int64_t> * result){
	vector<Slice> refined_slice;
	int dim = S->at(0).level; // Current dimension of slices in S
    int64_t low = rangequeries->at(dim).at(0);
    int64_t high = rangequeries->at(dim).at(1);
	int i = binarySearch (S,low);
//			distance(S->begin(),lower_bound(S->begin(), S->end(), low,compareBoxKey()));
	while (i < S->size() and S->at(i).box_end <= high){
		if ((S->at(i).box_begin < low && S->at(i).box_end < low) || (S->at(i).box_begin > high && S->at(i).box_end > high)){
			vector<Slice> refined_slice_aux = refine(S->at(i),table,rangequeries);
			for (int j = 0; j <  refined_slice_aux.size(); j++ ){
				if (((refined_slice_aux[i].box_begin < low && refined_slice_aux[i].box_end < low) || (refined_slice_aux[i].box_begin > high && refined_slice_aux[i].box_end > high))){
					if(refined_slice_aux[j].level == NUMBER_OF_COLUMNS){
						for (int k = refined_slice_aux[j].data_offset_begin; k <= refined_slice_aux[j].data_offset_end; k++){
							if(!(table->columns[dim][k] < low && table->columns[dim][k] > high))
								result->push_back(table->columns[dim][k]);
						}
					}
					else{
						// I Guess this just initialize the pointers to the childs?
//						if(refined_slice_aux[j].refined_slices->size() > 0){
//							createDefaultChild(&refined_slice_aux[j]);
//						}
						query(rangequeries,table,refined_slice_aux[j].refined_slices, result);
					}
				}
			}	
			refined_slice.insert(refined_slice.end(), refined_slice_aux.begin(), refined_slice_aux.end());
			i++;
		}

	}
	S->insert(S->end(), refined_slice.begin(), refined_slice.end());
	sort(S->begin(), S->end(), less_than_offset());
}

void quasii_pre_processing(Table *table, vector<Slice> *S){
	calculate_level_thresholds();
     for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
    {
        table->crackercolumns[j] = (IndexEntry *)malloc(COLUMN_SIZE * sizeof(IndexEntry));
        for (size_t i = 0; i < COLUMN_SIZE; ++i)
        {
            table->crackercolumns[j][i].m_key = table->columns[j][i];
            table->crackercolumns[j][i].m_rowId = table->ids[i];
        }
    }
    Slice *slice = new Slice();
    S->push_back(*slice);
}




