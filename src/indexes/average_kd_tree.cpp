#include "kd_node.hpp"
#include "full_scan.hpp"
#include "average_kd_tree.hpp"

AverageKDTree::AverageKDTree(){}
AverageKDTree::~AverageKDTree(){}

void AverageKDTree::initialize(const shared_ptr<Table> table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table, as this is the cracking_table
    table = make_unique<Table>(table_to_copy);

    // Initialize KD-Tree with average
    index = initialize_index();

    measurements->initialization_time = measurements->time() - start;
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

shared_ptr<Table> AverageKDTree::range_query(Query& query){
    // ******************
    auto start = measurements->time();

    // Search on the index the correct partitions
    auto partitions = index->search(query);

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
    measurements->number_of_nodes.push_back(index->get_node_count());
    measurements->max_height.push_back(index->get_max_height());
    measurements->min_height.push_back(index->get_min_height());
    measurements->memory_footprint.push_back(index->get_node_count() * sizeof(KDNode));

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

    nodes_to_check.push_back(index->root);
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

        if(current->left_position - lower_limit > minimum_partition_size){
            auto average_result = find_average(column, lower_limit, current->left_position);
            auto average = average_result.first;
            auto position = average_result.second;

            if(!(position < lower_limit || position >= current->left_position)){
                current->left_child = index->create_node(column, average, position);

                nodes_to_check.push_back(current->left_child);
                columns.push_back(column);
                lower_limits.push_back(lower_limit);
                upper_limits.push_back(current->left_position);
            }
        }

        if(upper_limit - current->right_position > minimum_partition_size){
            auto average_result = find_average(column, current->right_position, upper_limit);
            auto average = average_result.first;
            auto position = average_result.second;

            if(!(position < current->right_position || position >= upper_limit)){
                current->right_child = index->create_node(column, average, position);

                nodes_to_check.push_back(current->right_child);
                columns.push_back(column);
                lower_limits.push_back(current->right_position);
                upper_limits.push_back(upper_limit);
            }
        }
    }

    return index;
}

pair<float, size_t> AverageKDTree::find_average(size_t column, size_t lower_limit, size_t upper_limit){
    float sum = 0.0;
    for (size_t i = lower_limit; i <= upper_limit; i++)
        sum += table->columns.at(column)->at(i);

    auto average = sum/static_cast<float>(upper_limit-lower_limit+1);

    auto position = table->CrackTable(lower_limit, upper_limit, average, column);

    return make_pair(average, position-1);
}