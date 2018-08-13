using namespace std;
int MAX_LEVEL = 0;

const int last_level_threshold = 60;
vector<int> dimension_threshold;
struct less_than_offset
{
    inline bool operator() (const Slice& s1, const Slice& s2)
    {
        return (s1.data_offset_begin < s2.data_offset_begin);
    }
};

struct less_than_box
{
    inline bool operator() (const Slice& s1, const Slice& s2)
    {
        return (s1.box_begin] < s2.box_begin);
    }
};


// Sort the slices based on the lower offset

void query(vector<array<int64_t, 3>>  *rangequeries, Table *table, vector<Slice> *S, vector<int64_t> * result){
	vector<Slice> refined_slice;
    int64_t low = rangequeries->at(0).at(0);
    int64_t high = rangequeries->at(0).at(1);
    int64_t col = rangequeries->at(0).at(2);
	int dim = S[0].level; // Current dimension of slices in S
	auto i = (int) lower_bound(S.begin(), S.end(), low,less_than_box());
	while (i < S.Size() and S[i].box_end <= high){
		if (intersect(low,high,box_begin,box_end)){
			vector<Slice> refined_slice_aux = refine(S[i],table,rangequeries);
			for (int j = 0; j <  refined_slice_aux.size(); j++ ){
				if (intersect(low,high,box_begin,box_end)){
					if(refined_slice_aux[j].l == MAX_LEVEL){
						for (int k = refined_slice_aux[j].data_offset_begin; k <= refined_slice_aux[j].data_offset_end; k++){
							if(intersect(low,high,table[dim][k]))
								result->push_back(table[dim][k]);
						}
					}
					else{
						if(!refined_slice_aux[j].refined_slices){
							createDefaultChild(refined_slice_aux[j])
						}
						query(rangequeries,table,refined_slice_aux[j].refined_slices, result);
					}
				}
			}	
			refined_slice.insert(refined_slice.end(), refined_slice_aux.begin(), refined_slice_aux.end());
			i++;
		}

	}
	S.insert(S.end(), refined_slice.begin(), refined_slice.end());
	sort(S.begin(), S.end(), less_than_offset());
}

void refine(Slice S, Table *table, vector<array<int64_t, 3>>  *rangequeries){
    int64_t low = rangequeries->at(0).at(0);
    int64_t high = rangequeries->at(0).at(1);
    int64_t col = rangequeries->at(0).at(2);
	if (S.size() <= threshold)
		return S;
 	vector<Slice> refined_slice;
 	if (S.box_begin <= low && S.box_end >= high ) // lower and high are within box
 		refined_slice = sliceThreeWays();
 	else if (S.box_begin <= low && S.box_end < high )
 		refined_slice = sliceTwoWays();
 	else if (S.box_begin > low && S.box_end >= high )
 		refined_slice = sliceTwoWays();
 	else
 		refined_slice = sliceArtificial();

}

void calculate_level_thresholds(){

}