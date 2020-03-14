#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "median_kd_tree.hpp"

MedianKDTree::MedianKDTree(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
MedianKDTree::~MedianKDTree(){}


void MedianKDTree::initialize(Table *table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<IdxTbl>(table_to_copy);

    // Initialize KD-Tree with medians
    initialize_index();

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );    // ******************
}

void MedianKDTree::adapt_index(Table *originalTable,Query& query){
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

Table MedianKDTree::range_query(Table *originalTable,Query& query){
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

void MedianKDTree::initialize_index(){
    index = make_unique<KDTree>(table->row_count());
    auto median_result_root = find_median(0, 0, table->row_count());
    auto median_root = median_result_root.first;
    auto position_root = median_result_root.second;

    index->root = index->create_node(0, median_root, position_root);

    initialize_index_recursion(index->root.get(), 0, table->row_count(), 0);
}

void MedianKDTree::initialize_index_recursion(
    KDNode* current, int64_t lower_limit, int64_t upper_limit, int64_t column
){
    auto new_col = (column + 1) % table->col_count();
    if(current->position - lower_limit > minimum_partition_size){
        auto average_result = find_median(column, lower_limit, current->position);
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
        auto average_result = find_median(column, current->position, upper_limit);
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

pair<float, int64_t> MedianKDTree::find_median(int64_t column, int64_t lower_limit, int64_t upper_limit){
    int64_t low = lower_limit;
    int64_t high = upper_limit - 1;
    int64_t position;
    float element;

    do{
        element = table->columns[column]->data[(high+low)/2];
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
        if (table->columns[column]->data[position - 1] != table->columns[column]->data[position])
            break;
    }

    return make_pair(element, position);
}

// Returns the position on where the pivot would end
int64_t MedianKDTree::pivot_table(int64_t column, int64_t low, int64_t high, float pivot, int64_t pivot_position)
{
//  This method only works if we use the last element as the pivot
//  So we change the pivot to the last position
    table->exchange(pivot_position, high);

    int64_t i = low - 1;

    for (int64_t j = low; j < high; ++j)
    {
        if (table->columns[column]->data[j] <= pivot)
        {
            ++i;
            table->exchange(i, j);
        }
    }
    table->exchange(i + 1, high);
    return i + 1;
}
