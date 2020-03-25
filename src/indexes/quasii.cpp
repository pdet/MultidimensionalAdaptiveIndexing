#include "quasii.hpp"
#include "full_scan.hpp"
#include <math.h>
#include <algorithm>
#include <stack>
#include <fstream>
#include <limits>

using namespace std;

Quasii::Quasii(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        last_level_threshold  = 100;
    else
        last_level_threshold  = std::stoi(config["minimum_partition_size"]);
}
Quasii::~Quasii(){}

void Quasii::initialize(Table *table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    // Initialize with one Slice that covers all the data
    first_level_slices.push_back(
        Slice(0, 0, table->row_count())
    );

    // Calculate the thresholds
    calculate_level_thresholds();

    auto end = measurements->time();

        measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
    // ******************
}

void Quasii::adapt_index(Table *originalTable,Query& query){
    // Before adapting calculate the scan overhead to measure how much the previous
    // queries helped this one
    auto search_results= search(query);
    auto partitions = search_results.first;
    auto partition_skip = search_results.second;
    n_tuples_scanned_before_adapting = 0;
    for(auto &partition : partitions)
        n_tuples_scanned_before_adapting += partition.second - partition.first;

    // ******************
    auto start = measurements->time();

    build(first_level_slices, query);

    auto end = measurements->time();
    // ******************
    measurements->append(
        "adaptation_time",
        std::to_string(Measurements::difference(end, start))
    );
}

unique_ptr<Table>  Quasii::range_query(Table *originalTable,Query& query){
    // ******************
    auto start = measurements->time();

    // Search on the index the correct partitions
    auto search_results= search(query);
    auto partitions = search_results.first;
    auto partition_skip = search_results.second;

    auto end = measurements->time();
    measurements->append(
        "index_search_time",
        std::to_string(Measurements::difference(end, start))
    );

    start = measurements->time();
    // Scan the table and returns the row ids 
    auto result = FullScan::scan_partition(table.get(), query,partitions, partition_skip);

    end = measurements->time();
    // ******************
    measurements->append(
        "scan_time",
        std::to_string(Measurements::difference(end, start))
    );

    int64_t n_tuples_scanned = 0;
    for(auto &partition : partitions)
        n_tuples_scanned += partition.second - partition.first;

    // Before returning the result, update the statistics.
    measurements->append(
        "number_of_nodes",
        std::to_string(count_slices(first_level_slices))
    );
    measurements->append("max_height", std::to_string(table->col_count()));
    measurements->append("min_height", std::to_string(table->col_count()));
    measurements->append(
        "memory_footprint",
        std::to_string(count_slices(first_level_slices) * sizeof(Slice))
    );
    measurements->append("tuples_scanned", std::to_string(n_tuples_scanned));
    measurements->append("partitions_scanned", std::to_string(partitions.size()));

    auto skips = 0;
    for(auto i = 0; i < partition_skip.size(); ++i){
        if(partition_skip.at(i)){
            skips += 1;
        }
    }
    measurements->append("partitions_skipped", std::to_string(skips));
    measurements->append(
        "scan_overhead_before_adapt",
        std::to_string(
            n_tuples_scanned_before_adapting/static_cast<float>(result->row_count())
        )
    );

    measurements->append(
        "scan_overhead_after_adapt",
        std::to_string(
            n_tuples_scanned/static_cast<float>(result->row_count())
        )
    );

    return result;
}

int64_t Quasii::count_slices(std::vector<Slice> &slices){
    int64_t number_of_slices = slices.size();
    for(auto& slice : slices){
        number_of_slices += count_slices(slice.children);
    }
    return number_of_slices;
}

void Quasii::search_recursion(
    Slice &slice,
    Query &query,
    vector<pair<int64_t, int64_t>> &partitions,
    vector<bool> &partition_skip,
    vector<pair<float, float>> partition_borders
){
    if(slice.children.empty()){
        partitions.push_back(
                make_pair(slice.offset_begin, slice.offset_end)
                );
        partition_skip.push_back(
                query.covers(partition_borders)
                );
    }else{
        auto predicate = query.predicates[slice.children[0].column];
        auto i = binarySearch(slice.children, predicate.low);

        while(i < static_cast<int64_t>(slice.children.size()) && slice.children[i].left_value <= predicate.high){
            partition_borders.at(slice.column).first = slice.left_value;
            partition_borders.at(slice.column).second = slice.right_value;
            search_recursion(slice.children[i], query, partitions, partition_skip, partition_borders);
            ++i;
        }
    }
}

pair<vector<pair<int64_t, int64_t>>, vector<bool>> Quasii::search(Query& query){
    std::vector<pair<int64_t, int64_t>> partitions;
    std::vector<bool> partition_skip;

    auto predicate = query.predicates[first_level_slices[0].column];
    auto i = binarySearch(first_level_slices, predicate.low);

    vector<pair<float, float>> partition_borders(query.predicate_count());
    for(size_t i = 0; i < query.predicate_count(); ++i){
        partition_borders.at(i) = make_pair(
                numeric_limits<float>::lowest(),
                numeric_limits<float>::max()
                );
    }
    while(i < static_cast<int64_t>(first_level_slices.size()) && first_level_slices[i].left_value <= predicate.high){
        search_recursion(first_level_slices[i], query, partitions, partition_skip, partition_borders);
        ++i;
    }
    return make_pair(partitions, partition_skip);
}

// biggest who is less or equal to the key
int64_t Quasii::binarySearch(const std::vector<Slice> &slice, float key){
    auto  min = (int64_t) 0;
    auto  max = static_cast<int64_t>(slice.size() - 1);

    if(min == max)
        return min;

    while (max >= min && max > 0) {
        int64_t mid = ((max+min)/2.0) + 0.5;

        if(slice[mid].left_value ==  key){
            return mid;
        }else if(slice[mid].left_value < key){
            min = mid +1;
        }else if(slice[mid].left_value > key){
            max = mid -1;
        }

    }
    if(min < max)
        return min;
    return max;
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

void Quasii::build(std::vector<Slice> &slices, Query &query){
    std::vector<Slice> refined_slice_aux;
    auto dim = slices[0].column;
    auto predicate = query.predicates[dim];
    auto low = predicate.low;
    auto high = predicate.high;
    auto i = binarySearch(slices, low);
    auto index_start = i;
    while (i < static_cast<int64_t>(slices.size()) && slices[i].left_value <= high){
        std::vector<Slice> refined_slices = refine(slices[i], predicate);
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
        refined_slice_aux.insert(
            refined_slice_aux.end(),
            std::make_move_iterator(refined_slices.begin()),
            std::make_move_iterator(refined_slices.end())
        );
        i++;
    }

    slices.erase(slices.begin() + index_start, slices.begin() + i);

    slices.insert(
        slices.end(),
        std::make_move_iterator(refined_slice_aux.begin()),
        std::make_move_iterator(refined_slice_aux.end())
    );
    sort(slices.begin(), slices.end(), less_than_offset());
}


std::vector<Slice> Quasii::refine(Slice &slice, Predicate &predicate){
    auto low = predicate.low;
    auto high = predicate.high;
    std::vector<Slice> result_slices;
    std::vector<Slice> refined_slices;
    // If the slice size is below the threshold then dont refine it
    if ((slice.offset_end - slice.offset_begin) <= dimensions_threshold[slice.column]){
        refined_slices.push_back(std::move(slice));
        return refined_slices;
    }

    // lower and high are within box
    if (slice.left_value <= low && high <= slice.right_value){
        refined_slices = sliceThreeWay(slice, low, high);
    }
    else if (slice.left_value <= low && low < slice.right_value){
        refined_slices = sliceTwoWay(slice, low);
    }
    else if (slice.left_value < high && high <= slice.right_value ){
        refined_slices = sliceTwoWay(slice, high);
    }
    else{
        refined_slices = sliceArtificial(slice);
    }
    if(table->col_count() == 1){
//      It is not necessary to refine the created slices because there is no children
//      Otherwise, the creation cost may become too high, since the first column of slices will have a small threshold
//      resulting in a lot of sliceArtificial calls.
        return refined_slices;
    }
    for (auto &r_s : refined_slices){
        if(r_s.size() > dimensions_threshold[r_s.column] && r_s.intersects(low, high)){
            std::vector<Slice> refined_slice_aux = sliceArtificial(r_s);
        result_slices.insert(
            result_slices.end(),
            std::make_move_iterator(refined_slice_aux.begin()),
            std::make_move_iterator(refined_slice_aux.end())
        );
        }
        else{
            result_slices.push_back(std::move(r_s));
        }
    }
    return result_slices;

}

// This case there is no pivot in the slice, but it is inside the query
// Slice:
//          |===|
//          |===|
// Query: |---------|
//
// So it is necessary to crack using artificial pivots.
// Choose the middle one based on the left and right values of the slice.
// Keep cracking the new slices until the threshold is achieved.
// Crack the child of the new slices also.
std::vector<Slice> Quasii::sliceArtificial(Slice &slice){
    std::vector<Slice> result;
    stack<Slice> slices_to_be_refined;
    auto threshold = dimensions_threshold[slice.column];

    if(table->col_count() == 1){
        if(slice.size() > threshold)
            return sliceTwoWay(slice, (slice.right_value + slice.left_value)/2);
        else
            return result;
    }

    slices_to_be_refined.push(std::move(slice));

    do{
        Slice& slice_ = slices_to_be_refined.top();
        slices_to_be_refined.pop();

        if(slice_.size() < threshold)
            result.push_back(std::move(slice_));
        else{
            std::vector<Slice> slices_refined = sliceTwoWay(
                slice_, (slice_.right_value + slice_.left_value)/2.0
            );
            for(auto &s : slices_refined){
                if(s.equal(slice))
                    result.push_back(std::move(s));
                else
                    slices_to_be_refined.push(std::move(s));
            }
        }

    } while(!slices_to_be_refined.empty());

    return result;
}

// Same as Cracking-in-Two
//         |==================|
//         |                  |
//         |                  |
//         |                  |
//         |                  |
//         |==================|
//                  ||
//                Pivot
//
// Results into two slices:
//         |========||==========|
//         |        ||          |
//         |        ||          |
//         |        ||          |
//         |        ||          |
//         |========||==========|

std::vector<Slice> Quasii::sliceTwoWay(Slice &slice, float key){
    std::vector<Slice> result;

    auto pivot_index = table->CrackTable(
        slice.offset_begin, slice.offset_end, key, slice.column
    );

    // It might be the case that the pivot_index is equal to the offset_begin
    // Then instead of creating a new slice, just extend the other one
    if(slice.offset_begin >= pivot_index){
        pivot_index = slice.offset_begin;
    }else{
        result.push_back(
            Slice(
                slice.column, slice.offset_begin, pivot_index, slice.left_value, key
                )
            );
    }
    // If the pivot index is greater of equal to the offset_end
    // then we shouldn't insert it
    if(!(pivot_index >= slice.offset_end)){
        result.push_back(
                Slice(
                    slice.column, pivot_index, slice.offset_end, key, slice.right_value
                    )
                );
    }

    return result;
}

// Same as Cracking-in-Three
//         |==================|
//         |                  |
//         |                  |
//         |                  |
//         |                  |
//         |==================|
//            ||          ||
//          Pivot1      Pivot2
//
// Results into three slices:
//         |========||===||=======|
//         |        ||   ||       |
//         |        ||   ||       |
//         |        ||   ||       |
//         |        ||   ||       |
//         |========||===||=======|

std::vector<Slice> Quasii::sliceThreeWay(Slice &slice, float low, float high){
    std::vector<Slice> result;

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
    }
    else{
        // First partition is empty, so fix the indexes to use on the middle one
        first_crack = slice.offset_begin;
    }

    // Check if last partition is empty
    if(second_crack < slice.offset_end){
        result.push_back(
            Slice(slice.column, second_crack, slice.offset_end, high, slice.right_value)
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

void Quasii::draw_index(std::string path){
    std::ofstream myfile(path.c_str());

    myfile << "digraph Quasii{\n node [shape=record];\n";

    std::vector<std::vector<Slice>*> slices;
    slices.push_back(&first_level_slices);

    while(!slices.empty()){
        std::vector<Slice> *array_of_slices = slices.back();
        slices.pop_back();

        auto array_id = std::to_string(
                    reinterpret_cast<size_t>(&((*array_of_slices)[0]))
                );

        // First we create the node
        myfile << array_id + "[label=\"\n";
        for(auto &slice : *array_of_slices){
            myfile << "<" + std::to_string(reinterpret_cast<size_t>(&slice)) + ">";
            myfile << slice.label();
            myfile << "|";
        }

        myfile << "\"\n];\n";

        // Then we link the nodes
        for(size_t i = 0; i < array_of_slices->size(); ++i){
            auto &slice = array_of_slices->at(i);
            auto slice_id = std::to_string(
                    reinterpret_cast<size_t>(&(slice))
                );
            if(slice.children.empty()){}
            else{
                myfile << array_id + ":" + array_id + "->" + std::to_string(
                    reinterpret_cast<size_t>(&(slice.children[0]))
                ) + ";\n";
                slices.push_back(&(slice.children));
            }
        }
    }

    myfile << "\n}";
    myfile.close();
}

