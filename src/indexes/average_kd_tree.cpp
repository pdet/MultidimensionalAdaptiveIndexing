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
    table = make_unique<Table>(table_to_copy);

    // Initialize KD-Tree with average
    index = initialize_index();

    auto end = measurements->time();

    measurements->initialization_time = Measurements::difference(end, start);
    // ******************
}

void AverageKDTree::adapt_index(Query& query){
    // ******************
    auto start = measurements->time();
    // DOES ABSOLUTELY NOTHING HERE
    auto end = measurements->time();
    // ******************
    measurements->adaptation_time.push_back(
        Measurements::difference(end, start)
    );
}

Table AverageKDTree::range_query(Query& query){
    // ******************
    auto start = measurements->time();

    // Search on the index the correct partitions
    auto partitions = index->search(query);

    // Scan the table and returns a materialized view of the result.
    auto result = Table(table->col_count());
    for (auto partition : partitions)
    {
        auto low = partition.first;
        auto high = partition.second;
        FullScan::scan_partition(table.get(), query, low, high, &result);
    }

    auto end = measurements->time();
    // ******************
    measurements->query_time.push_back(
        Measurements::difference(end, start)
    );

    int64_t n_tuples_scanned = 0;
    for(auto &partition : partitions)
        n_tuples_scanned += partition.second - partition.first;

    // Before returning the result, update the statistics.
    measurements->number_of_nodes.push_back(index->get_node_count());
    measurements->max_height.push_back(index->get_max_height());
    measurements->min_height.push_back(index->get_min_height());
    measurements->memory_footprint.push_back(index->get_node_count() * sizeof(KDNode));
    measurements->tuples_scanned.push_back(n_tuples_scanned);

    return result;
}

unique_ptr<KDTree> AverageKDTree::initialize_index(){
    unique_ptr<KDTree> index = make_unique<KDTree>(table->row_count());
    auto average_result_root = find_average(0, 0, table->row_count() - 1);
    auto average_root = average_result_root.first;
    auto position_root = average_result_root.second;

    index->root = index->create_node(0, average_root, position_root);

    nodes_to_check.resize(0);
    lower_limits.resize(0);
    upper_limits.resize(0);
    columns.resize(0);

    nodes_to_check.push_back(index->root.get());
    lower_limits.push_back(0);
    upper_limits.push_back(table->row_count() - 1);
    columns.push_back(0);

    while(!nodes_to_check.empty()){
        auto current = nodes_to_check.back();
        nodes_to_check.pop_back();

        auto lower_limit = lower_limits.back();
        lower_limits.pop_back();

        auto upper_limit = upper_limits.back();
        upper_limits.pop_back();

        auto column = (columns.back() + 1) % table->col_count();
        columns.pop_back();

        if(current->position - lower_limit > minimum_partition_size){
            auto average_result = find_average(column, lower_limit, current->position - 1);
            auto average = average_result.first;
            auto position = average_result.second;

            if(!(position < lower_limit || position >= current->position - 1)){
                current->left_child = index->create_node(column, average, position);

                nodes_to_check.push_back(current->left_child.get());
                columns.push_back(column);
                lower_limits.push_back(lower_limit);
                upper_limits.push_back(current->position - 1);
            }
        }

        if(upper_limit - current->position > minimum_partition_size){
            auto average_result = find_average(column, current->position, upper_limit);
            auto average = average_result.first;
            auto position = average_result.second;

            if(!(position < current->position || position >= upper_limit)){
                current->right_child = index->create_node(column, average, position);

                nodes_to_check.push_back(current->right_child.get());
                columns.push_back(column);
                lower_limits.push_back(current->position);
                upper_limits.push_back(upper_limit);
            }
        }
    }

    return index;
}

pair<float, int64_t> AverageKDTree::find_average(int64_t column, int64_t lower_limit, int64_t upper_limit){
    float sum = 0.0;
    for (int64_t i = lower_limit; i <= upper_limit; i++)
        sum += table->columns.at(column)->at(i);

    auto average = sum/static_cast<float>(upper_limit-lower_limit+1);

    auto position = table->CrackTable(lower_limit, upper_limit, average, column);

    return make_pair(average, position);
}
