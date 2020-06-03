#include "quasii.hpp"
#include "full_scan.hpp"
#include <math.h>
#include <algorithm>
#include <stack>
#include <fstream>
#include <limits>
#include <full_scan_candidate_list.hpp>

using namespace std;

Quasii::Quasii(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        last_level_threshold  = 100;
    else
        last_level_threshold  = std::stoi(config["minimum_partition_size"]);
}
Quasii::~Quasii(){}

void Quasii::initialize(Table *table_to_copy){
    //! Check partition size, we change it to guarantee it always partitions all dimensions at least once
    while (last_level_threshold > table_to_copy->row_count()/pow(2,table_to_copy->col_count())){
        last_level_threshold /=2;
    }
    if (last_level_threshold < 100){
        last_level_threshold = 100;
    }
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    // Initialize with one Slice that covers all the data
    first_level_slices.push_back(
        createDefaultChild(0, 0, table->row_count())
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

void Quasii::adapt_index(Query &query) {
    // Before adapting calculate the scan overhead to measure how much the previous
    // queries helped this one
    auto search_results= search(query);
    auto partitions = search_results.first;
    n_tuples_scanned_before_adapting = 0;
    for(auto &partition : partitions)
        n_tuples_scanned_before_adapting += partition.second - partition.first;

    if(should_adapt){
        if(has_converged()){
            should_adapt = false;
            auto start = measurements->time();
            auto end = start;
            // ******************
            measurements->append(
                    "adaptation_time",
                    std::to_string(Measurements::difference(end, start))
                    );

        }else{
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

    }else{
        // ******************
        auto start = measurements->time();
        auto end = start;
        // ******************
        measurements->append(
                "adaptation_time",
                std::to_string(Measurements::difference(end, start))
                );
    }

}

unique_ptr<Table> Quasii::range_query(Query &query) {
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
    auto result = FullScanCandidateList::scan_partition(table.get(), query,partitions, partition_skip);

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
    for(size_t i = 0; i < partition_skip.size(); ++i){
        if(partition_skip.at(i)){
            skips += 1;
        }
    }
    measurements->append("partitions_skipped", std::to_string(skips));
    measurements->append(
            "scan_overhead_before_adapt",
            std::to_string(
                n_tuples_scanned_before_adapting/static_cast<float>(result.second)
                )
            );

    measurements->append(
            "scan_overhead_after_adapt",
            std::to_string(
                n_tuples_scanned/static_cast<float>(result.second)
                )
            );

    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(result.first), static_cast<float>(result.second)};
    t->append(row);
    return t;
}

size_t Quasii::count_slices(std::vector<Slice> &slices){
    int64_t number_of_slices = slices.size();
    for(auto& slice : slices){
        number_of_slices += count_slices(slice.children);
    }
    return number_of_slices;
}

void Quasii::search_recursion(
        Slice &slice,
        Query &query,
        vector<pair<size_t, size_t>> &partitions,
        vector<bool> &partition_skip,
        vector<pair<float, float>> partition_borders
        ){
    if(slice.children.empty()){
        partitions.push_back(
                make_pair(slice.offset_begin, slice.offset_end)
                );
        partition_borders.at(slice.column).first = slice.left_value;
        partition_borders.at(slice.column).second = slice.right_value;
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

pair<vector<pair<size_t, size_t>>, vector<bool>> Quasii::search(Query& query){
    std::vector<pair<size_t, size_t>> partitions;
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
                                createDefaultChild(r_s.column + 1, r_s.offset_begin,r_s.offset_end)
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


void Quasii::sliceArtificialRecursion(Slice& slice, std::vector<Slice>& result){
    auto threshold = dimensions_threshold[slice.column];
    if(slice.size() < threshold){
        result.push_back(std::move(slice));
        return;
    }

    std::vector<Slice> slices_refined = sliceTwoWay(
            slice, (slice.right_value + slice.left_value)/2.0
            );

    // The slicing had no effect, so stop slicing
    if(slices_refined.size() < 2){
        result.push_back(std::move(slices_refined[0]));
        return;
    }

    for(auto& s : slices_refined){
        sliceArtificialRecursion(s, result);
    }
}
std::vector<Slice> Quasii::sliceArtificial(Slice &slice){
    std::vector<Slice> result;
    auto threshold = dimensions_threshold[slice.column];

    if(slice.size() < threshold){
        result.push_back(std::move(slice));
        return result;
    }

    sliceArtificialRecursion(slice, result);

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
    // If the pivot index is greater or equal to the offset_end
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

    if(first_crack - slice.offset_begin > 0){
        result.push_back(
                Slice(slice.column, slice.offset_begin, first_crack, slice.left_value, low)
                );
    }

    if(second_crack - first_crack > 0){
        result.push_back(
                Slice(slice.column, first_crack, second_crack, low, high)
                );
    }

    if(slice.offset_end - second_crack > 0){
        result.push_back(
                Slice(slice.column, second_crack, slice.offset_end, high, slice.right_value)
                );
    }
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

bool Quasii::sanity_check_recursion(Slice& slice, vector<pair<float, float>> &borders){
    borders.at(slice.column) = make_pair(slice.left_value, slice.right_value);
    if(slice.column == table->col_count() - 1){
        Query query(borders);
        vector<pair<size_t, size_t>> partition;
        partition.push_back(make_pair(slice.offset_begin, slice.offset_end));
        vector<bool> partition_skip (1, false);
        auto result = FullScan::scan_partition(table.get(), query, partition, partition_skip);
        return result.second == (slice.offset_end - slice.offset_begin);
    }else{
        for(auto& child : slice.children){
            auto result = sanity_check_recursion(child, borders);
            if(result == false)
                return false;
        }
    }
    return true;
}

bool Quasii::sanity_check(){
    for(auto& slice : first_level_slices){
        vector<pair<float, float>> borders(table->col_count());
        auto result = sanity_check_recursion(slice, borders);
        if(result == false)
            return false;
    }
    return true;
}

bool Quasii::has_converged_recursion(Slice& slice){
    if(slice.column == table->col_count() - 1){
        return (slice.offset_end - slice.offset_begin) <= last_level_threshold;
    }else{
        // If we are not a leaf, and have no children then we did not converge
        if(slice.children.size() == 0)
            return false;
        for(auto& child : slice.children){
            auto result = has_converged_recursion(child);
            if(result == false)
                return false;
        }
    }
    return true;
}
bool Quasii::has_converged(){
    for(auto& slice : first_level_slices){
        auto result = has_converged_recursion(slice);
        if(result == false)
            return false;
    }
    return true;
}
