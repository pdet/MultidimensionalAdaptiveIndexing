#include "quasii.hpp"
#include "full_scan.hpp"
#include <math.h>
#include <algorithm>
#include <stack>

Quasii::Quasii(){}
Quasii::~Quasii(){}

void Quasii::initialize(const shared_ptr<Table> table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    // Initialize with one Slice that covers all the data
    slices.push_back(
        Slice(0, 0, table->row_count() - 1)
    );

    // Calculate the thresholds
    calculate_level_thresholds();

    measurements->initialization_time = measurements->time() - start;
    // ******************
}

void Quasii::adapt_index(Query& query){
    // ******************
    auto start = measurements->time();

    build(slices, query);

    auto end = measurements->time();
    // ******************
    measurements->adaptation_time.push_back(
        Measurements::difference(end, start)
    );
}

shared_ptr<Table> Quasii::range_query(Query& query){
    // ******************
    auto start = measurements->time();

    // Search on the index the correct partitions
    auto partitions = search(query);

    // Scan the table and returns a materialized view of the result.
    auto result = make_shared<Table>(table->col_count());
    for (auto partition : partitions)
    {
        auto low = partition.first;
        auto high = partition.second;
        FullScan::scan_partition(table, query, low, high, result);
    }

    auto end = measurements->time();
    // ******************
    measurements->query_time.push_back(
        Measurements::difference(end, start)
    );

    // Before returning the result, update the statistics.
    measurements->number_of_nodes.push_back(count_slices(slices));
    measurements->max_height.push_back(table->col_count());
    measurements->min_height.push_back(table->col_count());
    measurements->memory_footprint.push_back(count_slices(slices) * sizeof(Slice));

    return result;
}

size_t Quasii::count_slices(vector<Slice> &slices){
    size_t number_of_slices = slices.size();
    for(auto slice : slices){
        number_of_slices += count_slices(slice.children);
    }
    return number_of_slices;
}

vector<pair<size_t, size_t>> Quasii::search(Query& query){
    vector<pair<size_t, size_t>> partitions;
    vector<Slice> slices_to_check;

    if(column_in_query(slices.at(0).column, query)){
        auto predicate = predicate_on_column(slices.at(0).column, query);
        auto i = binarySearch(slices, predicate.low);

        while(i < slices.size() && slices.at(i).left_value < predicate.high){
            slices_to_check.push_back(slices.at(i));
            ++i;
        }
    }else
        for(auto slice : slices)
            slices_to_check.push_back(slice);

    while (!slices_to_check.empty())
    {
        auto slice = slices_to_check.back();
        slices_to_check.pop_back();

        if(slice.children.empty()){
            partitions.push_back(
                make_pair(slice.offset_begin, slice.offset_end)
            );
        }else{
            if(column_in_query(slice.children.at(0).column, query)){
                auto predicate = predicate_on_column(slice.children.at(0).column, query);
                auto i = binarySearch(slice.children, predicate.low);

                while(i < slice.children.size() && slice.children.at(i).left_value < predicate.high){
                    slices_to_check.push_back(slice.children.at(i));
                    ++i;
                }
            }else
                for(auto slice : slice.children)
                    slices_to_check.push_back(slice);
        }
    }
    return partitions;
}

Predicate Quasii::predicate_on_column(size_t column, Query& query){
    for(auto predicate : query.predicates){
        if(column == predicate.column)
            return predicate;
    }
    return Predicate();
}

// biggest who is less or equal to the key
size_t Quasii::binarySearch(const vector<Slice> &S, float key){
    auto  min = 0;
    auto  max = S.size() - 1;

    if(min == max)
        return min;

    while (max >= min) {
        size_t mid = ((max+min)/2.0) + 0.5;

        if(S.at(mid).left_value ==  key){
            return mid;
        }else if(S.at(mid).left_value < key){
            min = mid +1;
        }else if(S.at(mid).left_value > key){
            max = mid -1;
        }

    }
    if(min < max)
        return min;
    return max;
}

bool Quasii::column_in_query(size_t column, Query &query){
    for(size_t i = 0; i < query.predicate_count(); ++i){
        if(column == query.predicates.at(i).column)
            return true;
    }
    return false;
}

void Quasii::calculate_level_thresholds(){
    if(table->col_count() == 1){
        dimensions_threshold.push_back(last_level_threshold);
        return;
    }
    dimensions_threshold.push_back(last_level_threshold);
    auto number_of_columns = static_cast<double>(table->col_count());
    auto number_of_rows = static_cast<double>(table->row_count());
    auto r = ceil(pow((double)number_of_rows/last_level_threshold, (double) (1.0/number_of_columns)));


    auto cur_thr = r * last_level_threshold;
    dimensions_threshold.push_back(cur_thr);
    for (auto i = 2; i  < number_of_columns; i ++){
        cur_thr = r * cur_thr;
        dimensions_threshold.push_back(cur_thr);
    }
    reverse(dimensions_threshold.begin(),dimensions_threshold.end());
}

struct less_than_offset
{
    inline bool operator() (const Slice& s1, const Slice& s2)
    {
        return (s1.offset_begin < s2.offset_begin);
    }
};

void Quasii::build(vector<Slice> &Slices, Query &rangequeries){
    vector<Slice> refined_slice_aux;
    auto dim = Slices.at(0).column;
    auto low = rangequeries.predicates.at(dim).low;
    auto high = rangequeries.predicates.at(dim).high;
    auto i = binarySearch(Slices, low);
    auto index_start = i;
    while (i < Slices.size() && Slices.at(i).left_value <= high){
        vector<Slice> refined_slices = refine(Slices.at(i), rangequeries);
        for (size_t j = 0; j < refined_slices.size(); j++ ){
            if(refined_slices.at(j).intersects(low, high)){
                if(refined_slices.at(j).column == table->col_count() - 1)
                    continue;
                else{
                    if(refined_slices[j].children.size() == 0){
                        refined_slices.at(j).children.push_back(
                            Slice(refined_slices.at(j).column + 1, refined_slices.at(j).offset_begin,refined_slices.at(j).offset_end)
                        );
                    }
                    build(refined_slices.at(j).children, rangequeries);
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

vector<Slice> Quasii::sliceArtificial(Slice &S){
    vector<Slice> result;
    stack<Slice> slices_to_be_refined;
    auto threshold = dimensions_threshold.at(S.column);

    if(table->col_count() == 1){
        if(S.size() > threshold)
            return sliceTwoWay(S, (S.right_value + S.left_value)/2);
        else
            return result;
    }

    slices_to_be_refined.push(S);

    do{
        Slice slice = slices_to_be_refined.top();
        slices_to_be_refined.pop();

        if(slice.size() > threshold){
            vector<Slice> slices_refined = sliceTwoWay(
                slice, (S.right_value + S.left_value)/2
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

// Same as Cracking-in-Two
vector<Slice> Quasii::sliceTwoWay(Slice &S, float key){
    vector<Slice> result;

    auto pivot_index = table->CrackTable(S.offset_begin, S.offset_end, key, S.column);

    if(S.offset_begin < pivot_index - 1)
        result.push_back(
                Slice(S.column, S.offset_begin, pivot_index - 1, S.left_value, key)
        );
    if(pivot_index < S.offset_end)
        result.push_back(
                Slice(S.column, pivot_index, S.offset_end, key, S.right_value)
        );

    return result;
}

// Same as Cracking-in-Three
vector<Slice> Quasii::sliceThreeWay(Slice &S, float low, float high){
    vector<Slice> result;

    auto crack_result = table->CrackTableInThree(S.offset_begin, S.offset_end, low, high, S.column);
    auto first_crack = crack_result.first;
    auto second_crack = crack_result.second;

    // Check if first partition is empty
    if(S.offset_begin < first_crack){
        result.push_back(
            Slice(S.column, S.offset_begin, first_crack, S.left_value, low)
        );
        first_crack++;
    }
    else{
        // First partition is empty, so fix the indexes to use on the middle one
        first_crack = S.offset_begin;
    }

    // Check if last partition is empty
    if(second_crack + 1 < S.offset_end){
        result.push_back(
            Slice(S.column, second_crack + 1, S.offset_end, high, S.right_value)
        );
    }
    else{
        second_crack = S.offset_end;
    }

    result.push_back(
        Slice(S.column, first_crack, second_crack, low, high)
    );

    return result;
}

// vector<Slice> Quasii::sliceThreeWay(Slice &S, float low, float high){
//     vector<Slice> result;

//     auto first_crack = table->CrackTable(S.offset_begin, S.offset_end, low, S.column);
//     first_crack--;
//     if(S.offset_begin < first_crack){
//         result.push_back(
//                 Slice(S.column, S.offset_begin, first_crack, S.left_value, low)
//         );
//         first_crack++;
//     }

//     auto second_crack = table->CrackTable(first_crack, S.offset_end, high, S.column);
//     second_crack--;

//     if(first_crack < second_crack){
//         result.push_back(
//                 Slice(S.column, first_crack, second_crack, low, high)
//         );
//         second_crack++;
//     }
//     if(second_crack < S.offset_end)
//         result.push_back(
//                 Slice(S.column, second_crack, S.offset_end, high, S.right_value)
//         );

//     return result;
// }

vector<Slice> Quasii::refine(Slice &slice, Query &rangequeries){
    auto low = rangequeries.predicates.at(slice.column).low;
    auto high = rangequeries.predicates.at(slice.column).high;
    vector<Slice> result_slices;
    vector<Slice> refined_slice;
    if ((slice.offset_end - slice.offset_begin) <= dimensions_threshold.at(slice.column)){
        refined_slice.push_back(slice);
        return refined_slice;
    }

     if (slice.left_value <= low && high <= slice.right_value) // lower and high are within box
         refined_slice = sliceThreeWay(slice, low, high);
     else if (slice.left_value <= low && low < slice.right_value)
         refined_slice = sliceTwoWay(slice, low);
     else if (slice.left_value < high && high <= slice.right_value )
         refined_slice = sliceTwoWay(slice, high);
     else
         refined_slice = sliceArtificial(slice);
     if(table->col_count() == 1){
//      It is not necessary to refine the created slices because there is no children
//      Otherwise, the creation cost may become too high, since the first column of slices will have a small threshold
//      resulting in a lot of sliceArtificial calls.
         return refined_slice;
     }
     for (size_t i = 0; i < refined_slice.size(); i ++){
         if(refined_slice.at(i).size() > dimensions_threshold.at(refined_slice.at(i).column) && refined_slice.at(i).intersects(low, high)){
             vector<Slice> refined_slice_aux = sliceArtificial(refined_slice.at(i));
            result_slices.insert(result_slices.end(), refined_slice_aux.begin(), refined_slice_aux.end());
         }
         else{
             result_slices.push_back(refined_slice.at(i));
         }
     }
     return result_slices;

}