#include "quasii.h"

using namespace std;

extern int64_t NUMBER_OF_COLUMNS,COLUMN_SIZE;


const int last_level_threshold = 60; // No clue how to set up this parameter for highly dimensional queries.
vector<int64_t> dimension_threshold;

// Caculate threshold of each level
void calculate_level_thresholds(){
	dimension_threshold.push_back(last_level_threshold);
	double root_aux = 1/NUMBER_OF_COLUMNS;
	int64_t r = ceil(pow(COLUMN_SIZE/last_level_threshold, (double) 1.0/NUMBER_OF_COLUMNS));
	

	int64_t cur_thr = r * last_level_threshold;
	dimension_threshold.push_back(cur_thr);
	for (int i = 2; i  < NUMBER_OF_COLUMNS; i ++){
		cur_thr = r * cur_thr;
		dimension_threshold.push_back(cur_thr);
	}
	reverse(dimension_threshold.begin(),dimension_threshold.end());

	for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
		fprintf(stderr, "%ld\n", dimension_threshold[i] );
}
struct less_than_offset
{
    inline bool operator() (const Slice& s1, const Slice& s2)
    {
        return (s1.data_offset_begin < s2.data_offset_begin);
    }
};

// biggest who is less or equal to the key
int binarySearch(vector<Slice> *S, int64_t key){
	int64_t  min = 0;
	int64_t  max = S->size();

	while (max >= min) {
		int mid = ((max+min)/2.0) + 0.5;

		if(S->at(mid).box_begin ==  key){
			return mid;
		}else if(S->at(mid).box_begin < key){
			min = mid +1;
		}else if(S->at(mid).box_begin > key){
			max = mid -1;
		}

	}
	if(min < max) return min;
	return max;
}

void exchange(Table *t, int64_t x1, int64_t x2)
{
    if (x1 == x2)
        return;

    for (size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        IndexEntry tmp = t->crackercolumns[i][x1];
        t->crackercolumns[i][x1] = t->crackercolumns[i][x2];
        t->crackercolumns[i][x2] = tmp;
    }
}

// Same as Cracking-in-Three
vector<Slice> sliceThreeWay(Slice &S, Table *table, int64_t low, int64_t high){
	vector<Slice> result;

	int64_t x1 = S.data_offset_begin, x2 = S.data_offset_end;
	int64_t c = S.level;
    while (x2 > x1 && table->crackercolumns[c][x2].m_key  >= high)
        x2--;
    int64_t x3 = x2;
    while (x3 > x1 && table->crackercolumns[c][x3].m_key  >= low)
    {
        if (table->crackercolumns[c][x3].m_key >= high)
        {
            exchange(table, x2, x3);
            x2--;
        }
        x3--;
    }
    while (x1 < x3)
    {
        if (table->crackercolumns[c][x1].m_key  < low)
            x1++;
        else
        {
            exchange(table, x1, x3);
            while (x3 > x1 && table->crackercolumns[c][x3].m_key  >= low)
            {
                if (table->crackercolumns[c][x3].m_key  >= high)
                {
                    exchange(table, x2, x3);
                    x2--;
                }
                x3--;
            }
        }
    }

	// Slice *s1 = new Slice(S.level, S.data_offset_begin, x2, S.box_begin, low);
	S.data_offset_end = x2;
	S.box_end = low;

	Slice *s2 = new Slice(S.level, x2 + 1, x3, low, high);
	Slice *s3 = new Slice(S.level, x3 + 1, S.data_offset_end, high, S.box_end);

	// result.push_back(*s1);
	result.push_back(*s2);
	result.push_back(*s3);

	return result;
}
// Same as Cracking-in-Two
vector<Slice> sliceTwoWay(Slice &S, Table *table, int64_t key){
	vector<Slice> result;

	int64_t x1 = S.data_offset_begin;
    int64_t x2 = S.data_offset_end;
	int64_t c = S.level;

    while (x1 <= x2)
    {
        if (table->crackercolumns[c][x1].m_key < key)
            x1++;
        else
        {
            while (x2 >= x1 && (table->crackercolumns[c][x2].m_key >= key))
                x2--;
            if (x1 < x2)
            {
                exchange(table, x1, x2);
                x1++;
                x2--;
            }
        }
    }
    if (x1 < x2)
        printf("Not all elements were inspected!");
    x1--;

	// Slice *s1 = new Slice(S.level, S.data_offset_begin, x1, S.box_begin, key);
	S.data_offset_end = x1;
	S.box_end = key;
	Slice *s2 = new Slice(S.level, x1 + 1, S.data_offset_end, key, S.box_end);

	// result.push_back(*s1);
	result.push_back(*s2);

	return result;
}

vector<Slice> sliceArtificial(Slice &S, Table *table){
	vector<Slice> result;
	return result;
}

vector<Slice> refine(Slice &S, Table *table, vector<array<int64_t, 3>>  *rangequeries){
    int64_t low = rangequeries->at(S.level).at(0);
    int64_t high = rangequeries->at(S.level).at(1);
	vector<Slice> result_slices;
	vector<Slice> refined_slice;
	if ((S.data_offset_end - S.data_offset_begin) <= dimension_threshold[S.level]){
		refined_slice.push_back(S);
		return refined_slice;
	}

 	if (S.box_begin <= low && high <= S.box_end) // lower and high are within box
 		refined_slice = sliceThreeWay(S, table, low, high);
 	else if (S.box_begin <= low && low < S.box_end)
 		refined_slice = sliceTwoWay(S, table, low);
 	else if (S.box_begin < high && high <= S.box_end )
 		refined_slice = sliceTwoWay(S, table, high);
 	else
 		refined_slice = sliceArtificial(S, table);
 	for (size_t i = 0; i < refined_slice.size(); i ++){
	    low = rangequeries->at(S.level).at(0);
    	high = rangequeries->at(S.level).at(1);
 		if(refined_slice[i].bigger_than_threshold(dimension_threshold[refined_slice[i].level]) && refined_slice[i].intersects(low, high)){
 			vector<Slice> refined_slice_aux = sliceArtificial(refined_slice[i], table);
			result_slices.insert(result_slices.end(), refined_slice_aux.begin(), refined_slice_aux.end());
 		}
 		else{
 			result_slices.push_back(refined_slice[i]);
 		}
 	}
 	return result_slices;

}
// Sort the slices based on the lower offset
void createDefaultChild(Slice *s){
	vector<Slice> children;
	Slice *slice = new Slice(
		s->level + 1, s->data_offset_begin, s->data_offset_end, s->box_begin, s->box_end
	);
	children.push_back(*slice);
	s->children = children;
}

void query(vector<array<int64_t, 3>>  *rangequeries, Table *table, vector<Slice> *S, vector<int64_t> * result){
	vector<Slice> refined_slice_aux;
	int dim = S->at(0).level; // Current dimension of slices in S
    int64_t low = rangequeries->at(dim).at(0);
    int64_t high = rangequeries->at(dim).at(1);
	int64_t i = binarySearch (S,low);
	while (i < S->size() && S->at(i).box_begin <= high){
		vector<Slice> refined_slices = refine(S->at(i), table,rangequeries);
		for (size_t j = 0; j < refined_slices.size(); j++ ){
			if(refined_slices[j].intersects(low, high)){
				if(refined_slices[j].isBottomLevel(NUMBER_OF_COLUMNS - 1)){
					for (int k = refined_slices[j].data_offset_begin; k <= refined_slices[j].data_offset_end; k++){
						if((table->columns[dim][k] <= low && table->columns[dim][k] < high))
							result->push_back(table->columns[dim][k]);
					}
				}
				else{
					if(refined_slices[j].children.size() > 0){
						createDefaultChild(&refined_slices[j]);
					}
					query(rangequeries,table,&refined_slices[j].children, result);
				}
			}
		}	
		refined_slice_aux.insert(refined_slice_aux.end(), refined_slices.begin(), refined_slices.end());
		i++;
	}
	S->insert(S->end(), refined_slice_aux.begin(), refined_slice_aux.end());
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




