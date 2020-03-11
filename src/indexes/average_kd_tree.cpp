#include "kd_node.hpp"
#include "full_scan.hpp"
#include "average_kd_tree.hpp"

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
    table = make_unique<IdxTbl>(table_to_copy);

    // Initialize KD-Tree with average
    initialize_index();

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
    // ******************
}

void AverageKDTree::adapt_index(Query& query){
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

Table AverageKDTree::range_query(Query& query){
    // ******************
auto start = measurements->time();

    // Search on the index the correct partitions
    auto partitions = index->search(query);

    auto end = measurements->time();
    measurements->append(
            "index_search_time",
            std::to_string(Measurements::difference(end, start))
            );

    start = measurements->time();
    // Scan the table and returns the row ids 
    auto result = Table(1);
    for (auto partition : partitions)
    {
        auto low = partition.first;
        auto high = partition.second;
        FullScan::scan_partition(table.get(), query, low, high, &result);
    }

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

    measurements->append(
        "scan_overhead",
        std::to_string(
            n_tuples_scanned/static_cast<float>(result.row_count())
        )
    );
    return result;
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
    KDNode* current, int64_t lower_limit, int64_t upper_limit, int64_t column
){
    auto new_col = (column + 1) % table->col_count();
    if(current->position - lower_limit > minimum_partition_size){
        auto average_result = find_average(new_col, lower_limit, current->position);
        auto average = average_result.first;
        auto position = average_result.second;

        if(position > lower_limit && position < upper_limit - 1){
            current->left_child = index->create_node(new_col, average, position);

            initialize_index_recursion(
                    current->left_child.get(),
                    lower_limit, current->position, 
                    new_col
                    );
        }
    }

    if(upper_limit - current->position > minimum_partition_size){
        auto average_result = find_average(new_col, current->position, upper_limit);
        auto average = average_result.first;
        auto position = average_result.second;

        if(position > lower_limit && position < upper_limit - 1){
            current->right_child = index->create_node(new_col, average, position);

            initialize_index_recursion(
                    current->right_child.get(),
                    current->position, upper_limit,
                    new_col
                    );
        }
    }
}

pair<float, int64_t> AverageKDTree::find_average(int64_t column, int64_t lower_limit, int64_t upper_limit){
    float sum = 0.0;
    for (int64_t i = lower_limit; i < upper_limit; i++)
        sum += table->columns[column]->data[i];

    auto average = sum/static_cast<float>(upper_limit-lower_limit+1);

    auto position = table->CrackTable(lower_limit, upper_limit, average, column);

    return make_pair(average, position);
}
