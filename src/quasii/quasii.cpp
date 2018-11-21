#include "quasii.h"

using namespace std;

extern int64_t NUMBER_OF_COLUMNS,COLUMN_SIZE;

static vector<Slice> S; //Static variable to hold all the slices on the first level


const int last_level_threshold = 2000; // No clue how to set up this parameter for highly dimensional queries.
vector<int64_t> dimension_threshold;

// Caculate threshold of each level
void calculate_level_thresholds(){
    if(NUMBER_OF_COLUMNS == 1){
        dimension_threshold.push_back(last_level_threshold);
        return;
    }
	dimension_threshold.push_back(last_level_threshold);
	double root_aux = 1.0/NUMBER_OF_COLUMNS;
	int64_t r = ceil(pow((double)COLUMN_SIZE/last_level_threshold, (double) 1.0/NUMBER_OF_COLUMNS));


	int64_t cur_thr = r * last_level_threshold;
	dimension_threshold.push_back(cur_thr);
	for (int i = 2; i  < NUMBER_OF_COLUMNS; i ++){
		cur_thr = r * cur_thr;
		dimension_threshold.push_back(cur_thr);
	}
	reverse(dimension_threshold.begin(),dimension_threshold.end());
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
	int64_t  max = S->size() - 1;

	if(min == max)
		return min;

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

void quasii_exchange(CrackerTable *t, int64_t x1, int64_t x2)
{
    if (x1 == x2)
        return;
    int64_t tmp;
    tmp = t->ids.at(x1);
    t->ids.at(x1) = t->ids.at(x2);
    t->ids.at(x2) = tmp;

    for (size_t i = 0; i < t->columns.size(); i++)
    {
        tmp = t->columns.at(i).at(x1);
        t->columns.at(i).at(x1) = t->columns.at(i).at(x2);
        t->columns.at(i).at(x2) = tmp;
    }
}

// Same as Cracking-in-Three
vector<Slice> sliceThreeWay(Slice &S, CrackerTable *table, int64_t low, int64_t high){
	vector<Slice> result;

	int64_t x1 = S.data_offset_begin, x2 = S.data_offset_end;
	int64_t c = S.level;
    while (x2 > x1 && table->columns.at(c).at(x2)  >= high)
        x2--;
    int64_t x3 = x2;
    while (x3 > x1 && table->columns.at(c).at(x3)  >= low)
    {
        if (table->columns.at(c).at(x3) >= high)
        {
            quasii_exchange(table, x2, x3);
            x2--;
        }
        x3--;
    }
    while (x1 < x3)
    {
        if (table->columns.at(c).at(x1)  < low)
            x1++;
        else
        {
            quasii_exchange(table, x1, x3);
            while (x3 > x1 && table->columns.at(c).at(x3)  >= low)
            {
                if (table->columns.at(c).at(x3)  >= high)
                {
                    quasii_exchange(table, x2, x3);
                    x2--;
                }
                x3--;
            }
        }
    }

	int64_t slice1_begin = S.data_offset_begin;
	int64_t slice1_end = x1;
	int64_t slice1_value_begin = S.box_begin;
	int64_t slice1_value_end = low;

	int64_t slice2_begin = x1 + 1;
	int64_t slice2_end = x2;
	int64_t slice2_value_begin = low;
	int64_t slice2_value_end = high;

	int64_t slice3_begin = x2 + 1;
	int64_t slice3_end = S.data_offset_end;
	int64_t slice3_value_begin = high;
	int64_t slice3_value_end = S.box_end;

    if(slice1_end <= slice1_begin){
            slice2_begin= slice1_begin;
            slice2_value_begin = slice1_value_begin;
    }else{
		result.push_back(
				*new Slice(S.level, slice1_begin, slice1_end, slice1_value_begin, slice1_value_end)
		);
    }

    if(slice3_end <= slice3_begin){
        slice2_end = slice3_end;
        slice2_value_end = slice3_value_end;
    }else{
        result.push_back(
                *new Slice(S.level, slice3_begin, slice3_end, slice3_value_begin, slice3_value_end)
        );
    }

    result.push_back(
            *new Slice(S.level, slice2_begin, slice2_end, slice2_value_begin, slice2_value_end)
    );


	return result;
}
// Same as Cracking-in-Two
vector<Slice> sliceTwoWay(Slice &S, CrackerTable *table, int64_t key){
	vector<Slice> result;

	int64_t x1 = S.data_offset_begin;
    int64_t x2 = S.data_offset_end;
	int64_t c = S.level;

    while (x1 <= x2)
    {
        if (table->columns.at(c).at(x1) < key)
            x1++;
        else
        {
            while (x2 >= x1 && (table->columns.at(c).at(x2) >= key))
                x2--;
            if (x1 < x2)
            {
                quasii_exchange(table, x1, x2);
                x1++;
                x2--;
            }
        }
    }
    if (x1 < x2)
        printf("Not all elements were inspected!");
    x1--;

    int64_t slice1_begin = S.data_offset_begin;
    int64_t slice1_end = x1;
    int64_t slice1_box_begin = S.box_begin;
    int64_t slice1_box_end = key;

    int64_t slice2_begin = x1 + 1;
    int64_t slice2_end = S.data_offset_end;
    int64_t slice2_box_begin = key;
    int64_t slice2_box_end = S.box_end;

    if(slice1_end <= slice1_begin){
        slice2_begin= slice1_begin;
        slice2_box_begin = slice1_box_begin;

        result.push_back(
                *new Slice(S.level, slice2_begin, slice2_end, slice2_box_begin, slice2_box_end)
        );
    }else if(slice2_end <= slice2_begin){
        slice1_end= slice2_end;
        slice1_box_end= slice2_box_end;

        result.push_back(
                *new Slice(S.level, slice1_begin, slice1_end, slice1_box_begin, slice1_box_end)
        );
    }else {
        result.push_back(
                *new Slice(S.level, slice2_begin, slice2_end, slice2_box_begin, slice2_box_end)
        );
        result.push_back(
                *new Slice(S.level, slice1_begin, slice1_end, slice1_box_begin, slice1_box_end)
        );
    }

	return result;
}

vector<Slice> sliceArtificial(Slice &S, CrackerTable *table){
	vector<Slice> result;
	stack<Slice> slices_to_be_refined;
	int64_t t = dimension_threshold[S.level];

	if(NUMBER_OF_COLUMNS == 1){
	    if(S.bigger_than_threshold(t))
	        return sliceTwoWay(S, table, (S.box_end + S.box_begin)/2);
	    else
	        return result;
	}

	slices_to_be_refined.push(S);

	do{
		Slice slice = slices_to_be_refined.top();
		slices_to_be_refined.pop();

		if(slice.bigger_than_threshold(t)){
			vector<Slice> slices_refined = sliceTwoWay(
				slice, table, (slice.box_end + slice.box_begin)/2
			);
			for(size_t i = 0; i < slices_refined.size(); ++i){
				if(slices_refined.at(i).equal(slice))
				    result.push_back(slices_refined.at(i));
                else
                    slices_to_be_refined.push(slices_refined.at(i));
			}
		}
		else
			result.push_back(slice);
	} while(!slices_to_be_refined.empty());

	return result;
}

// Sort the slices based on the lower offset
void createDefaultChild(Slice *s){
	vector<Slice> children;
	Slice *slice = new Slice(
		s->level + 1, s->data_offset_begin, s->data_offset_end, 0, COLUMN_SIZE
	);
	children.push_back(*slice);
	s->children = children;
}

vector<Slice> refine(Slice &slice, CrackerTable *table, vector<array<int64_t, 3>>  *rangequeries){
    int64_t low = rangequeries->at(slice.level).at(0);
    int64_t high = rangequeries->at(slice.level).at(1);
	vector<Slice> result_slices;
	vector<Slice> refined_slice;
	if ((slice.data_offset_end - slice.data_offset_begin) <= dimension_threshold[slice.level]){
		refined_slice.push_back(slice);
		return refined_slice;
	}

 	if (slice.box_begin <= low && high <= slice.box_end) // lower and high are within box
 		refined_slice = sliceThreeWay(slice, table, low, high);
 	else if (slice.box_begin <= low && low < slice.box_end)
 		refined_slice = sliceTwoWay(slice, table, low);
 	else if (slice.box_begin < high && high <= slice.box_end )
 		refined_slice = sliceTwoWay(slice, table, high);
 	else
 		refined_slice = sliceArtificial(slice, table);
 	if(NUMBER_OF_COLUMNS == 1){
//      It is not necessary to refine the created slices because there is no children
//      Otherwise, the creation cost may become too high, since the first level of slices will have a small threshold
//      resulting in a lot of sliceArtificial calls.
 	    return refined_slice;
 	}
 	for (size_t i = 0; i < refined_slice.size(); i ++){
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

// void query(vector<array<int64_t, 3>>  *rangequeries, Table *table, vector<Slice> *S, vector<int64_t> * result){
// 	vector<Slice> refined_slice_aux;
// 	int dim = S->at(0).level; // Current dimension of slices in S
//     int64_t low = rangequeries->at(dim).at(0);
//     int64_t high = rangequeries->at(dim).at(1);
// 	int64_t i = binarySearch (S,low);
// 	while (i < S->size() && S->at(i).box_begin <= high){
// 		vector<Slice> refined_slices = refine(S->at(i), table,rangequeries);
// 		for (size_t j = 0; j < refined_slices.size(); j++ ){
// 			if(refined_slices[j].intersects(low, high)){
// 				if(refined_slices[j].isBottomLevel(NUMBER_OF_COLUMNS - 1)){
// 					for (int k = refined_slices[j].data_offset_begin; k <= refined_slices[j].data_offset_end; k++){
// 						if((table->columns[dim][k] <= low && table->columns[dim][k] < high))
// 							result->push_back(table->columns[dim][k]);
// 					}
// 				}
// 				else{
// 					if(refined_slices[j].children.size() > 0){
// 						createDefaultChild(&refined_slices[j]);
// 					}
// 					query(rangequeries,table,&refined_slices[j].children, result);
// 				}
// 			}
// 		}
// 		refined_slice_aux.insert(refined_slice_aux.end(), refined_slices.begin(), refined_slices.end());
// 		i++;
// 	}
// 	S->insert(S->end(), refined_slice_aux.begin(), refined_slice_aux.end());
// 	sort(S->begin(), S->end(), less_than_offset());
// }

void partial_build(CrackerTable *table, vector<Slice> &Slices, vector<array<int64_t, 3>>  *rangequeries){
	vector<Slice> refined_slice_aux;
	int dim = Slices.at(0).level;
    int64_t low = rangequeries->at(dim).at(0);
    int64_t high = rangequeries->at(dim).at(1);
	int64_t i = binarySearch (&Slices, low);
	int64_t index_start = i;
	while (i < Slices.size() && Slices.at(i).box_begin <= high){
		vector<Slice> refined_slices = refine(Slices.at(i), table,rangequeries);
		for (size_t j = 0; j < refined_slices.size(); j++ ){
			if(refined_slices[j].intersects(low, high)){
				if(refined_slices[j].isBottomLevel(NUMBER_OF_COLUMNS - 1))
					continue;
				else{
					if(refined_slices[j].children.size() == 0){
						createDefaultChild(&refined_slices[j]);
					}
					partial_build(table, refined_slices[j].children, rangequeries);
				}
			}
		}
		refined_slice_aux.insert(refined_slice_aux.end(), refined_slices.begin(), refined_slices.end());
		i++;
	}

	Slices.erase(Slices.begin() + index_start, Slices.begin() + i);

	Slices.insert(Slices.end(), refined_slice_aux.begin(), refined_slice_aux.end());
	sort(Slices.begin(), Slices.end(), less_than_offset());
}

void lookup(vector<Slice> &Slices, vector<array<int64_t, 3>>  *rangequeries, vector<pair<int,int>>  *offsets){
	int dim = Slices.at(0).level;
    int64_t low = rangequeries->at(dim).at(0);
    int64_t high = rangequeries->at(dim).at(1);
	int64_t i = binarySearch (&Slices, low);
	while (i < Slices.size() && Slices.at(i).box_begin < high){
		if(Slices.at(i).intersects(low, high)){
			if(Slices.at(i).isBottomLevel(NUMBER_OF_COLUMNS - 1)){
				offsets->push_back(make_pair(
					Slices.at(i).data_offset_begin,
					Slices.at(i).data_offset_end
				));
			}
			else
				lookup(Slices.at(i).children, rangequeries, offsets);
		}
		i++;
	}
}

// The second argument is not used, its only there to conform with the interface defined in main.cpp
void quasii_pre_processing(Table *table, Tree * t){
	calculate_level_thresholds();
	table->crackertable.columns = vector<vector<int64_t> >(NUMBER_OF_COLUMNS);
    table->crackertable.ids = vector<int64_t>(COLUMN_SIZE);

	for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col)
    {
        table->crackertable.columns.at(col) = vector<int64_t>(COLUMN_SIZE);
        for (size_t line = 0; line < COLUMN_SIZE; ++line)
        {
            table->crackertable.ids.at(line) = table->ids.at(line);
            table->crackertable.columns.at(col).at(line) = table->columns.at(col).at(line);
        }
    }

    Slice *slice = new Slice();
    S.push_back(*slice);
}

void quasii_partial_built(Table *table, Tree * t, vector<array<int64_t, 3>>  *rangequeries){
	partial_build(&table->crackertable, S, rangequeries);
}

void quasii_index_lookup(Tree * t, vector<array<int64_t, 3>>  *rangequeries, vector<pair<int,int>>  *offsets){
	lookup(S, rangequeries, offsets);
}

bool verify_slice(Slice slice, CrackerTable *table){
    int64_t begin = slice.data_offset_begin;
    int64_t end = slice.data_offset_end;

    int64_t low = slice.box_begin;
    int64_t high = slice.box_end;

    for (int i = begin; i <= end; ++i) {
        if(!(low <= table->columns.at(slice.level).at(i) && table->columns.at(slice.level).at(i) < high))
            return false;
    }
    return true;
}

void verify_slices(vector<Slice> S, CrackerTable *table){
    for (size_t i = 0; i < S.size(); ++i) {
        if(!verify_slice(S.at(i), table)){
            fprintf(stderr, "Bad slice\n");
            fprintf(stderr, "Level: %d\n", S.at(i).level);
            fprintf(stderr, "Offset begin: %ld\n", S.at(i).data_offset_begin);
            fprintf(stderr, "Offset end: %ld\n", S.at(i).data_offset_end);
            fprintf(stderr, "Value begin: %ld\n", S.at(i).box_begin);
            fprintf(stderr, "Value end: %ld\n", S.at(i).box_end);
        }
        if(S.at(i).children.size() > 0)
            verify_slices(S.at(i).children, table);
    }
}

void quasii_scan(Table *table, vector<array<int64_t, 3>> *query, vector<pair<int,int>>  *offsets, vector<int64_t> * result){
//	verify_slices(S, &table->crackertable);
    for(size_t i = 0; i < offsets->size(); ++i){
        int sel_size;
        int sel_vector[offsets->at(i).second - offsets->at(i).first + 1];
        int64_t low = query->at(0).at(0);
		int64_t high = query->at(0).at(1);
		int64_t col = query->at(0).at(2);
        sel_size = select_rq_scan_new (sel_vector, &table->crackertable.columns[col][offsets->at(i).first],low,high,offsets->at(i).second - offsets->at(i).first + 1);
        for (size_t query_num = 1; query_num < query->size(); query_num++)
        {
            int64_t low = query->at(query_num).at(0);
			int64_t high = query->at(query_num).at(1);
			int64_t col = query->at(query_num).at(2);
            sel_size = select_rq_scan_sel_vec(sel_vector, &table->crackertable.columns[col][offsets->at(i).first],low,high,sel_size);
        }
        for (size_t j = 0; j < sel_size; ++j)
            result->push_back(table->crackertable.ids[sel_vector[j] + offsets->at(i).first]);
    }
}
