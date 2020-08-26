#include "cracking_kd_tree_pre_processing.hpp"
#include "kd_node.hpp"
#include "full_scan.hpp"
#include <algorithm> // to check if all elements of a vector are true
#include <limits>
#include <full_scan_candidate_list.hpp>

using namespace std;

CrackingKDTreePreProcessing::CrackingKDTreePreProcessing(std::map<std::string, std::string> config){

    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);

    if(config.find("interactive_threshold") == config.end())
        interactive_threshold = 0.5;
    else
        interactive_threshold = std::stoi(config["interactive_threshold"]);

    WRITE_ONE_PAGE_SEQ_MS = cost_model.write_sequential_page_cost();
    READ_ONE_PAGE_SEQ_MS = cost_model.read_sequential_with_matches_page_cost();
    READ_ONE_PAGE_WITHOUT_CHECKS_SEQ_MS =
            cost_model.read_sequential_without_matches_page_cost();
    RANDOM_ACCESS_PAGE_MS = cost_model.read_random_access();
    cost_model.swap_cost_create(first_col_cr, extra_col_cr);
}
CrackingKDTreePreProcessing::~CrackingKDTreePreProcessing(){}

void CrackingKDTreePreProcessing::initialize(Table *table_to_copy){
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

    // Pre build the index using averages to speed up first couple of queries
    initialize_index((0.5 / READ_ONE_PAGE_SEQ_MS) * cost_model.PAGESIZE);

    auto end = measurements->time();

    measurements->append(
            "initialization_time",
            std::to_string(Measurements::difference(end, start))
            );
    // ******************
}

void CrackingKDTreePreProcessing::adapt_index(Query &query) {
    // Before adapting calculate the scan overhead to measure how much the previous
    // queries helped this one
    auto search_results= index->search(query);
    auto partitions = search_results.first;
    n_tuples_scanned_before_adapting = 0;
    for(auto &partition : partitions)
        n_tuples_scanned_before_adapting += partition.second - partition.first;

    if(should_adapt){
        // If the index has converged then we stop adaptation
        if(index->has_converged(table.get(), minimum_partition_size)){
            should_adapt = false;
            // Add 0 to adaptation_time
            auto start = measurements->time();
            auto end = start;
            measurements->append(
                    "adaptation_time",
                    std::to_string(Measurements::difference(end, start))
                    );
        }else{
            // Adapt the KDTree 
            auto start = measurements->time();
            adapt(query);
            auto end = measurements->time();
            // ******************
            measurements->append(
                    "adaptation_time",
                    std::to_string(Measurements::difference(end, start))
                    );
        }
    }else{
        // Add 0 to adaptation_time
        auto start = measurements->time();
        auto end = start;
        measurements->append(
            "adaptation_time",
            std::to_string(Measurements::difference(end, start))
            );
    }
}

unique_ptr<Table> CrackingKDTreePreProcessing::range_query(Query &query) {
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

void CrackingKDTreePreProcessing::adapt(Query& query){

    if(index->root == nullptr){
        auto position = table->CrackTable(
                0, table->row_count(),
                query.predicates[0].low, 0 
                );
        index->root = index->create_node(0, query.predicates[0].low, position);
    }

    for(size_t dim = 0; dim < query.predicate_count(); ++dim){
        if(query.predicates[dim].low >= std::numeric_limits<float>::lowest())
            adapt_recursion(
                    index->root.get(),
                    query,
                    dim, query.predicates[dim].low,
                    0, table->row_count()
                    );
    }

    for(size_t dim = 0; dim < query.predicate_count(); ++dim){
        if(query.predicates[dim].high <= std::numeric_limits<float>::max())
            adapt_recursion(
                    index->root.get(),
                    query,
                    dim, query.predicates[dim].high,
                    0, table->row_count()
                    );
    }
}

void CrackingKDTreePreProcessing::adapt_recursion(
        KDNode *current,
        Query& query,
        size_t pivot_dim,
        float pivot,
        size_t lower_limit,
        size_t upper_limit
        ){

    // If the size of the partition is already too small then stop exploring it
    if(upper_limit - lower_limit + 1 < minimum_partition_size)
        return;

    // If that pivot has already been inserted then we don't need to
    // keep looking in this branch.
    if(current->column == pivot_dim && current->key == pivot){
        return;
    }

    // If the current node has the same column as the pivot dim
    if(current->column == pivot_dim){
        if(pivot < current->key){
            if(current->left_child == nullptr){
                auto position = table->CrackTable(
                        lower_limit, current->position,
                        pivot, pivot_dim
                        );
                current->left_child = index->create_node(
                        pivot_dim, pivot, position
                        );
            }else{
                adapt_recursion(
                        current->left_child.get(), query,
                        pivot_dim, pivot,
                        lower_limit, current->position
                        );
                return;
            }
        }else{
            if(current->right_child == nullptr){
                auto position = table->CrackTable(
                        current->position, upper_limit,
                        pivot, pivot_dim
                        );
                current->right_child = index->create_node(
                        pivot_dim, pivot, position
                        );
            }else{
                adapt_recursion(
                        current->right_child.get(), query,
                        pivot_dim, pivot,
                        current->position, upper_limit
                        );
                return;
            }
        }
    }else{
        switch(current->compare(query)){
            case -1:
                //                  Key
                // Data:  |----------!--------|
                // Query:            |-----|
                //                  low   high
                if(current->right_child == nullptr){
                    auto position = table->CrackTable(
                            current->position, upper_limit,
                            pivot, pivot_dim
                            );
                    current->right_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->right_child.get(), query,
                            pivot_dim, pivot,
                            current->position, upper_limit
                            );
                    return;
                }
                break;
            case +1:
                //                  Key
                // Data:  |----------!--------|
                // Query:      |-----|
                //            low   high
                if(current->left_child == nullptr){
                    auto position = table->CrackTable(
                            lower_limit, current->position,
                            pivot, pivot_dim
                            );
                    current->left_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->left_child.get(), query,
                            pivot_dim, pivot,
                            lower_limit, current->position
                            );
                    return;
                }
                break;
            case 0:
                if(current->left_child == nullptr){
                    auto position = table->CrackTable(
                            lower_limit, current->position,
                            pivot, pivot_dim
                            );
                    current->left_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->left_child.get(), query,
                            pivot_dim, pivot,
                            lower_limit, current->position
                            );
                }
                if(current->right_child == nullptr){
                    auto position = table->CrackTable(
                            current->position, upper_limit,
                            pivot, pivot_dim
                            );
                    current->right_child = index->create_node(
                            pivot_dim, pivot, position
                            );
                }else{
                    adapt_recursion(
                            current->right_child.get(), query,
                            pivot_dim, pivot,
                            current->position, upper_limit
                            );
                }
                break;
            default:
                assert(false);
        }
    }
}

void CrackingKDTreePreProcessing::initialize_index(size_t partition_size){
    index = make_unique<KDTree>(table->row_count());
    auto average_result_root = find_average(0, 0, table->row_count());
    auto average_root = average_result_root.first;
    auto position_root = average_result_root.second;

    index->root = index->create_node(0, average_root, position_root);

    initialize_index_recursion(index->root.get(), 0, table->row_count(), 0, partition_size);
}

void CrackingKDTreePreProcessing::initialize_index_recursion(
        KDNode* current, size_t lower_limit, size_t upper_limit, size_t column, size_t partition_size
){
    auto new_col = (column + 1) % table->col_count();
    if(current->position - lower_limit > partition_size){
        auto average_result = find_average(new_col, lower_limit, current->position);
        auto average = average_result.first;
        auto position = average_result.second;

        if(lower_limit < position && position < current->position){
            current->left_child = index->create_node(new_col, average, position);

            initialize_index_recursion(
                    current->left_child.get(),
                    lower_limit, current->position,
                    new_col, partition_size
            );
        }
    }

    if(upper_limit - current->position > partition_size){
        auto average_result = find_average(new_col, current->position, upper_limit);
        auto average = average_result.first;
        auto position = average_result.second;

        if(current->position < position && position < upper_limit){
            current->right_child = index->create_node(new_col, average, position);

            initialize_index_recursion(
                    current->right_child.get(),
                    current->position, upper_limit,
                    new_col, partition_size
            );
        }
    }
}

pair<float, size_t> CrackingKDTreePreProcessing::find_average(size_t column, size_t lower_limit, size_t upper_limit){
    float sum = 0.0;
    for (size_t i = lower_limit; i < upper_limit; i++)
        sum += table->columns[column]->data[i];

    auto average = sum/static_cast<float>(upper_limit-lower_limit+1);

    auto position = table->CrackTable(lower_limit, upper_limit, average, column);

    return make_pair(average, position);
}
