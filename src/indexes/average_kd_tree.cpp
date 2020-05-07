#include "average_kd_tree.hpp"
#include "kd_tree.hpp"
#include "full_scan.hpp"

using namespace std;

AverageKDTree::AverageKDTree(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
AverageKDTree::~AverageKDTree(){}

void AverageKDTree::initialize(Table *table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    // Initialize KD-Tree with average
    initialize_index();

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
    // ******************
}

void AverageKDTree::adapt_index(Table *originalTable,Query& query){
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

unique_ptr<Table>  AverageKDTree::range_query(Table *originalTable,Query& query){
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
    measurements->append("number_of_nodes", std::to_string(index->get_node_count()));
    measurements->append("max_height", std::to_string(index->get_max_height()));
    measurements->append("min_height", std::to_string(index->get_min_height()));
    measurements->append("memory_footprint", std::to_string(index->get_node_count() * sizeof(KDNode)));
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


void AverageKDTree::initialize_index(){
    index = make_unique<KDTree>(table->row_count());
    auto average_result_root = find_average(0, 0, table->row_count());
    auto average_root = average_result_root.first;
    auto position_root = average_result_root.second;

    index->root = index->create_node(0, average_root, position_root);

    initialize_index_recursion(index->root.get(), 0, table->row_count(), 0);
}

void AverageKDTree::initialize_index_recursion(
    KDNode* current, size_t lower_limit, size_t upper_limit, size_t column
){
    auto new_col = (column + 1) % table->col_count();
    if(current->position - lower_limit > minimum_partition_size){
        auto average_result = find_average(column, lower_limit, current->position);
        auto average = average_result.first;
        auto position = average_result.second;

        if(!(position < lower_limit || position >= current->position)){
            current->left_child = index->create_node(column, average, position);

            initialize_index_recursion(
                    current->left_child.get(),
                    lower_limit, current->position, 
                    new_col
                    );
        }
    }

    if(upper_limit - current->position > minimum_partition_size){
        auto average_result = find_average(column, current->position, upper_limit);
        auto average = average_result.first;
        auto position = average_result.second;

        if(!(position < current->position || position >= upper_limit)){
            current->right_child = index->create_node(column, average, position);

            initialize_index_recursion(
                    current->right_child.get(),
                    current->position, upper_limit,
                    new_col
                    );
        }
    }
}

pair<float, size_t> AverageKDTree::find_average(size_t column, size_t lower_limit, size_t upper_limit){
    float sum = 0.0;
    for (size_t i = lower_limit; i < upper_limit; i++)
        sum += table->columns[column]->data[i];

    auto average = sum/static_cast<float>(upper_limit-lower_limit+1);

    auto position = table->CrackTable(lower_limit, upper_limit, average, column);

    return make_pair(average, position);
}
