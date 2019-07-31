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
    first_level_slices.push_back(
        Slice(0, 0, table->row_count() - 1)
    );

    // Calculate the thresholds
    calculate_level_thresholds();

    auto end = measurements->time();

    measurements->initialization_time = Measurements::difference(end, start);
    // ******************
}

void Quasii::adapt_index(Query& query){
    // ******************
    auto start = measurements->time();

    build(first_level_slices, query);

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
    measurements->number_of_nodes.push_back(count_slices(first_level_slices));
    measurements->max_height.push_back(table->col_count());
    measurements->min_height.push_back(table->col_count());
    measurements->memory_footprint.push_back(count_slices(first_level_slices) * sizeof(Slice));

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

    if(column_in_query(first_level_slices.at(0).column, query)){
        auto predicate = predicate_on_column(first_level_slices.at(0).column, query);
        auto i = binarySearch(first_level_slices, predicate.low);

        while(i < first_level_slices.size() && first_level_slices.at(i).left_value < predicate.high){
            slices_to_check.push_back(first_level_slices.at(i));
            ++i;
        }
    }else
        for(auto slice : first_level_slices)
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
                slices_to_check.insert(
                    slices_to_check.end(),
                    slice.children.begin(),
                    slice.children.end()
                );
        }
    }
    return partitions;
}

Predicate Quasii::predicate_on_column(size_t column, Query& query){
    for(auto predicate : query.predicates){
        if(column == predicate.column)
            return predicate;
    }
    assert(false);
}

// biggest who is less or equal to the key
size_t Quasii::binarySearch(const vector<Slice> &slice, float key){
    auto  min = (size_t) 0;
    auto  max = slice.size() - 1;

    if(min == max)
        return min;

    while (max >= min && max > 0) {
        size_t mid = ((max+min)/2.0) + 0.5;

        if(slice.at(mid).left_value ==  key){
            return mid;
        }else if(slice.at(mid).left_value < key){
            min = mid +1;
        }else if(slice.at(mid).left_value > key){
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
    auto r = ceil(
                pow(
                    static_cast<double>(number_of_rows/last_level_threshold),
                    static_cast<double>(1.0/number_of_columns)
                )
            );

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

void Quasii::build(vector<Slice> &slices, Query &query){
    vector<Slice> refined_slice_aux;
    auto dim = slices.at(0).column;
    auto predicate = predicate_on_column(dim, query);
    auto low = predicate.low;
    auto high = predicate.high;
    auto i = binarySearch(slices, low);
    auto index_start = i;
    while (i < slices.size() && slices.at(i).left_value <= high){
        vector<Slice> refined_slices = refine(slices.at(i), predicate);
        for (auto &r_s : refined_slices){
            if(r_s.intersects(low, high)){
                if(r_s.column == table->col_count() - 1)
                    continue;
                else{
                    if(r_s.children.size() == 0){
                        r_s.children.push_back(
                            Slice(r_s.column + 1, r_s.offset_begin,r_s.offset_end)
                        );
                    }
                    build(r_s.children, query);
                }
            }
        }
        refined_slice_aux.insert(refined_slice_aux.end(), refined_slices.begin(), refined_slices.end());
        i++;
    }

    slices.erase(slices.begin() + index_start, slices.begin() + i);

    slices.insert(slices.end(), refined_slice_aux.begin(), refined_slice_aux.end());
    sort(slices.begin(), slices.end(), less_than_offset());
}

vector<Slice> Quasii::sliceArtificial(Slice &slice){
    vector<Slice> result;
    stack<Slice> slices_to_be_refined;
    auto threshold = dimensions_threshold.at(slice.column);

    if(table->col_count() == 1){
        if(slice.size() > threshold)
            return sliceTwoWay(slice, (slice.right_value + slice.left_value)/2);
        else
            return result;
    }

    slices_to_be_refined.push(slice);

    do{
        Slice slice_ = slices_to_be_refined.top();
        slices_to_be_refined.pop();

        if(slice_.size() < threshold)
            result.push_back(slice_);
        else{
            vector<Slice> slices_refined = sliceTwoWay(
                slice_, (slice_.right_value + slice_.left_value)/2.0
            );
            for(auto &s : slices_refined){
                if(s.equal(slice))
                    result.push_back(s);
                else
                    slices_to_be_refined.push(s);
            }
        }

    } while(!slices_to_be_refined.empty());

    return result;
}

// Same as Cracking-in-Two
vector<Slice> Quasii::sliceTwoWay(Slice &slice, float key){
    vector<Slice> result;

    auto pivot_index = table->CrackTable(slice.offset_begin, slice.offset_end, key, slice.column);

    if(slice.offset_begin + 1 < pivot_index)
        result.push_back(
                Slice(slice.column, slice.offset_begin, pivot_index - 1, slice.left_value, key)
        );
    if(pivot_index < slice.offset_end)
        result.push_back(
                Slice(slice.column, pivot_index, slice.offset_end, key, slice.right_value)
        );

    return result;
}

// Same as Cracking-in-Three
vector<Slice> Quasii::sliceThreeWay(Slice &slice, float low, float high){
    vector<Slice> result;

    auto crack_result = table->CrackTableInThree(
        slice.offset_begin, slice.offset_end, low, high, slice.column
    );
    auto first_crack = crack_result.first;
    auto second_crack = crack_result.second;

    // Check if first partition is empty
    if(slice.offset_begin < first_crack){
        result.push_back(
            Slice(slice.column, slice.offset_begin, first_crack, slice.left_value, low)
        );
        first_crack++;
    }
    else{
        // First partition is empty, so fix the indexes to use on the middle one
        first_crack = slice.offset_begin;
    }

    // Check if last partition is empty
    if(second_crack + 1 < slice.offset_end){
        result.push_back(
            Slice(slice.column, second_crack + 1, slice.offset_end, high, slice.right_value)
        );
    }
    else{
        second_crack = slice.offset_end;
    }

    result.push_back(
        Slice(slice.column, first_crack, second_crack, low, high)
    );

    return result;
}

vector<Slice> Quasii::refine(Slice &slice, Predicate &predicate){
    auto low = predicate.low;
    auto high = predicate.high;
    vector<Slice> result_slices;
    vector<Slice> refined_slices;
    if ((slice.offset_end - slice.offset_begin) <= dimensions_threshold.at(slice.column)){
        refined_slices.push_back(slice);
        return refined_slices;
    }

     if (slice.left_value <= low && high <= slice.right_value) // lower and high are within box
         refined_slices = sliceThreeWay(slice, low, high);
     else if (slice.left_value <= low && low < slice.right_value)
         refined_slices = sliceTwoWay(slice, low);
     else if (slice.left_value < high && high <= slice.right_value )
         refined_slices = sliceTwoWay(slice, high);
     else
         refined_slices = sliceArtificial(slice);
     if(table->col_count() == 1){
//      It is not necessary to refine the created slices because there is no children
//      Otherwise, the creation cost may become too high, since the first column of slices will have a small threshold
//      resulting in a lot of sliceArtificial calls.
         return refined_slices;
     }
     for (auto &r_s : refined_slices){
         if(r_s.size() > dimensions_threshold.at(r_s.column) && r_s.intersects(low, high)){
             vector<Slice> refined_slice_aux = sliceArtificial(r_s);
            result_slices.insert(result_slices.end(), refined_slice_aux.begin(), refined_slice_aux.end());
         }
         else{
             result_slices.push_back(r_s);
         }
     }
     return result_slices;

}