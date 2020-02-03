#include "abstract_cracking_kd_tree.hpp"
#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include <tuple>


AbstractCrackingKDTree::AbstractCrackingKDTree(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}

AbstractCrackingKDTree::~AbstractCrackingKDTree(){}

void AbstractCrackingKDTree::initialize(Table* table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table, as this is the cracking_table
    table = make_unique<Table>(table_to_copy);

    // Initialize KD-Tree
    index = make_unique<KDTree>(table->row_count());

    auto end = measurements->time();

    measurements->initialization_time = Measurements::difference(end, start);
    // ******************
}


Table AbstractCrackingKDTree::range_query(Query& query){
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

void AbstractCrackingKDTree::draw_index(std::string path){
    index->draw(path);
}

void AbstractCrackingKDTree::insert(Query &limits, int64_t column, float key){

    // If index->root is null
    //  crack on that partition
    //  create new node
    //  add it as root

    if(index->root == nullptr){
        auto position = table->CrackTable(0, table->row_count() - 1, key, column);
        index->root = index->create_node(column, key, position);
        return;
    }

    // reset the helper vectors
    // add root to nodes_to_check
    nodes_to_check.resize(0);
    lower_limits.resize(0);
    upper_limits.resize(0);

    nodes_to_check.push_back(index->root.get());
    lower_limits.push_back(0);
    upper_limits.push_back(table->row_count() - 1);

    while(!nodes_to_check.empty()){
        auto current = nodes_to_check.back();
        nodes_to_check.pop_back();

        auto lower_limit = lower_limits.back();
        lower_limits.pop_back();

        auto upper_limit = upper_limits.back();
        upper_limits.pop_back();

        if(current->column == column){
            if(current->key < key)
               follow_right_or_crack(current, limits, column, key, lower_limit); 
            if(current->key > key)
               follow_left_or_crack(current, limits, column, key, upper_limit); 
        }
        // If the node's key is greater or equal to the limit
        // Then follow the left child
        //                  Key
        // Data:  |----------!--------|
        // Limit:      |-----|
        //            low   high
        else if (current->node_greater_equal_query(limits)){
            follow_left_or_crack(current, limits, column, key, upper_limit);
        }
        // If the node's key is smaller to the low part of the query
        // Then follow the right child
        //                  Key
        // Data:  |----------!--------|
        // Limit:            |-----|
        //                  low   high
        else if (current->node_less_equal_query(limits)){
            follow_right_or_crack(current, limits, column, key, lower_limit); 
        }
        // If the node's key is inside the query
        // Then follow both children
        //                  Key
        // Data:  |----------!--------|
        // Limit:         |-----|
        //               low   high
        else{
            follow_right_or_crack(current, limits, column, key, lower_limit); 
            follow_left_or_crack(current, limits, column, key, upper_limit); 
        }
    }
}

// Checks the left child
// If it is null then we reached a partition
// Otherwise, we follow it
void AbstractCrackingKDTree::follow_left_or_crack(KDNode *current, Query& limits, int64_t column, float key, int64_t lower_limit){
    if (current->left_child != nullptr)
    {
        nodes_to_check.push_back(current->left_child.get());
        lower_limits.push_back(lower_limit);
        upper_limits.push_back(current->position - 1);
    }
    else
    {
        if(current->position - 1 - lower_limit > minimum_partition_size){
            // Crack Partition and create new node
            auto position = table->CrackTable(lower_limit, current->position - 1, key, column);
            current->left_child = index->create_node(column, key, position);
        }
    }
}

// Checks the right child
// If it is null then we reached a partition
// Otherwise, we follow it
void AbstractCrackingKDTree::follow_right_or_crack(KDNode *current, Query& limits, int64_t column, float key, int64_t upper_limit){
    if (current->right_child != nullptr)
    {
        nodes_to_check.push_back(current->right_child.get());
        lower_limits.push_back(current->position);
        upper_limits.push_back(upper_limit);
    }
    else
    {
        if(upper_limit - current->position> minimum_partition_size){
            // Crack Partition and create new node
            auto position = table->CrackTable(current->position, upper_limit, key, column);
            current->right_child = index->create_node(column, key, position);
        }
    }
}
