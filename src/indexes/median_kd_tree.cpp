#include <full_scan_candidate_list.hpp>
#include "median_kd_tree.hpp"
#include "kd_tree.hpp"
#include "full_scan.hpp"
#include <algorithm>

using namespace std;

MedianKDTree::MedianKDTree(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
MedianKDTree::~MedianKDTree(){}

void MedianKDTree::initialize(Table *table_to_copy){
        //! Check partition size, we change it to guarantee it always partitions all dimensions at least once
    while (minimum_partition_size > table_to_copy->row_count()/pow(2,table_to_copy->col_count())){
        minimum_partition_size /=2;
    }
    if (minimum_partition_size < 100){
        minimum_partition_size = 100;
    }
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    // Initialize KD-Tree with medians
    initialize_index();

    auto end = measurements->time();



    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
    // ******************
}

void MedianKDTree::adapt_index(Query &query) {
    // ******************
    auto start = measurements->time();
    // DOES ABSOLUTELY NOTHING HERE
    auto end = measurements->time();
    // ******************
    measurements->append(
        "adaptation_time",
        std::to_string(Measurements::difference(end, start))
    );
}

unique_ptr<Table> MedianKDTree::range_query(Query &query) {
    // ******************
    auto start = measurements->time();

    // Search on the index the correct partitions
    auto search_results= index->search(query);
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
    measurements->append("number_of_nodes", std::to_string(index->get_node_count()));
    measurements->append("max_height", std::to_string(index->get_max_height()));
    measurements->append("min_height", std::to_string(index->get_min_height()));
    measurements->append("memory_footprint", std::to_string(index->get_node_count() * sizeof(KDNode)));
    measurements->append("tuples_scanned", std::to_string(n_tuples_scanned));
    measurements->append("partitions_scanned", std::to_string(partitions.size()));

    auto skips = 0;
    for(size_t i = 0; i < partition_skip.size(); ++i){
        if(all_of(partition_skip.at(i).begin(), partition_skip.at(i).end(), [](bool v) { return v; })){
            skips += 1;
        }
    }
    measurements->append("partitions_skipped", std::to_string(skips));

    measurements->append(
        "scan_overhead",
        std::to_string(
            n_tuples_scanned/static_cast<float>(result.second)
        )
    );
    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(result.first), static_cast<float>(result.second)};
    t->append(row);
    return t;
}

void MedianKDTree::initialize_index(){
    index = make_unique<KDTree>(table->row_count());
    auto median_result_root = find_median(0, 0, table->row_count());
    auto median_root = median_result_root.first;
    auto position_root = median_result_root.second;

    index->root = index->create_node(0, median_root, position_root);

    initialize_index_recursion(index->root.get(), 0, table->row_count(), 0);
}

void MedianKDTree::initialize_index_recursion(
    KDNode* current, size_t lower_limit, size_t upper_limit, size_t column
){
    auto new_col = (column + 1) % table->col_count();
    if(current->position - lower_limit > minimum_partition_size){
        auto median_result = find_median(new_col, lower_limit, current->position);
        auto median = median_result.first;
        auto position = median_result.second;

        if(lower_limit < position && position < current->position){
            current->left_child = index->create_node(new_col, median, position);

            initialize_index_recursion(
                    current->left_child.get(),
                    lower_limit, current->position, 
                    new_col
                    );
        }
    }

    if(upper_limit - current->position > minimum_partition_size){
        auto median_result = find_median(new_col, current->position, upper_limit);
        auto median = median_result.first;
        auto position = median_result.second;

        if(current->position < position && position < upper_limit){
            current->right_child = index->create_node(new_col, median, position);

            initialize_index_recursion(
                    current->right_child.get(),
                    current->position, upper_limit,
                    new_col
                    );
        }
    }
}

pair<float, size_t > MedianKDTree::find_median(size_t column, size_t lower_limit, size_t upper_limit){
    auto copy = vector<float>(table->columns[column]->data + lower_limit, table->columns[column]->data + upper_limit);
    std::nth_element(copy.begin(), copy.begin() + copy.size()/2, copy.end());
    auto median = copy[copy.size()/2];

    auto position = table->CrackTable(lower_limit, upper_limit, median, column);

    return make_pair(median, position);
}
