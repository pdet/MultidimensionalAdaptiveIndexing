#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "median_kd_tree.hpp"

MedianKDTree::MedianKDTree(){}
MedianKDTree::~MedianKDTree(){}


void MedianKDTree::initialize(Table *table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    // Initialize KD-Tree with medians
    index = initialize_index();

    auto end = measurements->time();

    measurements->initialization_time = Measurements::difference(end, start);
    // ******************
}

void MedianKDTree::adapt_index(Query& query){
    // ******************
    auto start = measurements->time();
    // DOES ABSOLUTELY NOTHING HERE
    auto end = measurements->time();
    // ******************
    measurements->adaptation_time.push_back(
        Measurements::difference(end, start)
    );
}

Table MedianKDTree::range_query(Query& query){
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

    size_t n_tuples_scanned = 0;
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

unique_ptr<KDTree> MedianKDTree::initialize_index(){
    unique_ptr<KDTree> index = make_unique<KDTree>(table->row_count());
    auto median_result_root = find_median(0, 0, table->row_count() - 1);
    auto median_root = median_result_root.first;
    auto position_root = median_result_root.second;

    index->root = index->create_node(0, median_root, position_root);

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

        if(current->left_position - lower_limit > minimum_partition_size){
            auto median_result = find_median(column, lower_limit, current->left_position);
            auto median = median_result.first;
            auto position = median_result.second;

            if(!(position < lower_limit || position >= current->left_position)){
                current->left_child = index->create_node(column, median, position);

                nodes_to_check.push_back(current->left_child.get());
                columns.push_back(column);
                lower_limits.push_back(lower_limit);
                upper_limits.push_back(current->left_position);
            }
        }

        if(upper_limit - current->right_position > minimum_partition_size){
            auto median_result = find_median(column, current->right_position, upper_limit);
            auto median = median_result.first;
            auto position = median_result.second;

            if(!(position < current->right_position || position >= upper_limit)){
                current->right_child = index->create_node(column, median, position);

                nodes_to_check.push_back(current->right_child.get());
                columns.push_back(column);
                lower_limits.push_back(current->right_position);
                upper_limits.push_back(upper_limit);
            }
        }
    }

    return index;
}

pair<float, size_t> MedianKDTree::find_median(size_t column, size_t lower_limit, size_t upper_limit){
    size_t low = lower_limit;
    size_t high = upper_limit;
    size_t position;
    float element;

    do{
        element = table->columns.at(column)->at((high+low)/2);
        position = pivot_table(column, low, high, element, (high+low)/2);

        if (position <= low)
        {
            ++low;
        }
        else if (position >= high)
        {
            --high;
        }
        else
        {
            if (position < (lower_limit + upper_limit) / 2)
                low = position;
            else
                high = position;
        }
    }while (position != (lower_limit + upper_limit) / 2);

    // Loop in case the median is not unique, then get the first one
    for (; position > lower_limit; --position)
    {
        if (table->columns.at(column)->at(position - 1) != table->columns.at(column)->at(position))
            break;
    }

    return make_pair(element, position - 1);
}

// Returns the position on where the pivot would end
size_t MedianKDTree::pivot_table(size_t column, size_t low, size_t high, float pivot, size_t pivot_position)
{
//  This method only works if we use the last element as the pivot
//  So we change the pivot to the last position
    table->exchange(pivot_position, high);

    int64_t i = low - 1;

    for (int64_t j = low; j < high; ++j)
    {
        if (table->columns.at(column)->at(j) <= pivot)
        {
            ++i;
            table->exchange(i, j);
        }
    }
    table->exchange(i + 1, high);
    return i + 1;
}
