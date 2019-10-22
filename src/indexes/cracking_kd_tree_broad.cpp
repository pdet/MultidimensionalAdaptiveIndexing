#include "kd_tree.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree_broad.hpp"

CrackingKDTreeBroad::CrackingKDTreeBroad(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
CrackingKDTreeBroad::~CrackingKDTreeBroad(){}

void CrackingKDTreeBroad::initialize(Table *table_to_copy){
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

void CrackingKDTreeBroad::adapt_index(Query& query){
    // ******************
    auto start = measurements->time();

    for(auto predicate : query.predicates){
        insert(predicate.column, predicate.low);
        insert(predicate.column, predicate.high);
    }

    auto end = measurements->time();
    // ******************
    measurements->adaptation_time.push_back(
        Measurements::difference(end, start)
    );
}

Table CrackingKDTreeBroad::range_query(Query& query){
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

void CrackingKDTreeBroad::insert(int64_t column, float key){
    if(index->root == nullptr){
        // First insertion
        // Crack and insert into root
        int64_t lower_limit = 0;
        int64_t upper_limit = table->row_count() - 1;
        int64_t position = table->CrackTable(lower_limit, upper_limit, key, column);
        position--;
        if (!(position < lower_limit || position >= upper_limit)){
            index->root = index->create_node(column, key, position);
        }
        return;
    }
    // Search the partitions to crack based on column and key
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

        // Current node shares the same column
        if(current->column == column){
            // Current node is smaller than key to insert, then follow right
            // Current:      (col, key)
            //                         >
            // New:                     (col, k)
            if(current->key < key)
                follow_or_crack_right(current, column, key, upper_limit);
            // Current node is greater than key to insert
            // Current:       (col, key)
            //              <
            // New:   (col, k)
            else if(current->key > key)
                follow_or_crack_left(current, column, key, lower_limit);
            // Current node is equal to key to insert
            // Current:      (col, key)
            // New:          (col, key)
            else
                continue;
        }
        // Does not have the same column, them follow both children
        else{
            follow_or_crack_right(current, column, key, upper_limit);
            follow_or_crack_left(current, column, key, lower_limit);
        }
    }
}

void CrackingKDTreeBroad::follow_or_crack_right(KDNode *current, int64_t column, float key, int64_t upper_limit){
    // If the right child is null, then we crack that partition
    // Current:      (col, key)
    //              /          \
    // Child:                  null
    if(current->right_child == nullptr){
        if(upper_limit - current->right_position < minimum_partition_size)
            return;
        auto position = table->CrackTable(
            current->right_position, upper_limit,
            key, column
        );
        position--;
        if(!(position < current->right_position || position >= upper_limit)){
            current->right_child = index->create_node(
                column, key, position
            );
        }
    }
    // If the right child exists, then we follow it
    // Current:      (col, key)
    //              /          \
    // Child:                 (..., ...)
    else{
        nodes_to_check.push_back(current->right_child.get());
        lower_limits.push_back(current->right_position);
        upper_limits.push_back(upper_limit);
    }
}

void CrackingKDTreeBroad::follow_or_crack_left(KDNode *current, int64_t column, float key, int64_t lower_limit){
    // If the left child is null, then we crack that partition
    // Current:      (col, key)
    //              /          \
    // Child:     null
    if(current->left_child == nullptr){
        if(current->left_position - lower_limit < minimum_partition_size)
            return;
        auto position = table->CrackTable(
            lower_limit, current->left_position,
            key, column
        );
        position--;
        if(!(position < lower_limit || position >= current->left_position)){
            current->left_child = index->create_node(
                column, key, position
            );
        }
    }
    // If the left child exists, then we follow it
    // Current:      (col, key)
    //              /          \
    // Child: (..., ...)
    else{
        nodes_to_check.push_back(current->left_child.get());
        lower_limits.push_back(lower_limit);
        upper_limits.push_back(current->left_position);
    }
}
