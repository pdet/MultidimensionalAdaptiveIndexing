#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree_narrow.hpp"
#include <tuple>


CrackingKDTreeNarrow::CrackingKDTreeNarrow(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}

CrackingKDTreeNarrow::~CrackingKDTreeNarrow(){}

void CrackingKDTreeNarrow::initialize(Table* table_to_copy){
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

void CrackingKDTreeNarrow::adapt_index(Query& query){
    // ******************
    auto query_copy = Query(query);
    auto start = measurements->time();

    insert(query_copy);

    auto end = measurements->time();
    // ******************
    measurements->adaptation_time.push_back(
        Measurements::difference(end, start)
    );
}

Table CrackingKDTreeNarrow::range_query(Query& query){
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

void CrackingKDTreeNarrow::draw_index(std::string path){
    index->draw(path);
}

void CrackingKDTreeNarrow::insert(Query &original_query){
    auto query = make_unique<QueryShell>(&original_query);
    if(index->root == nullptr){
        index->root = insert_new_nodes(query.get(), 0, table->row_count() - 1);
        return;
    }

    nodes_to_check.resize(0);
    lower_limits.resize(0);
    upper_limits.resize(0);
    queries.resize(0);

    nodes_to_check.push_back(index->root.get());
    lower_limits.push_back(0);
    upper_limits.push_back(table->row_count() - 1);
    queries.push_back(query.get());
    while(!nodes_to_check.empty()){
        auto current = nodes_to_check.back();
        nodes_to_check.pop_back();

        auto lower_limit = lower_limits.back();
        lower_limits.pop_back();

        auto upper_limit = upper_limits.back();
        upper_limits.pop_back();

        auto current_query = queries.back();
        queries.pop_back();

        // If current's column is not in query follow both children
        if(!node_in_query(current, current_query)){
            insert_or_follow_left(current, lower_limit, upper_limit, current_query);
            insert_or_follow_right(current, lower_limit, upper_limit, current_query);
        }
        // If the node's key is greater or equal to the high part of the query
        // Then follow the left child
        //                  Key
        // Data:  |----------!--------|
        // Query:      |-----|
        //            low   high
        else if (node_greater_equal_query(current, current_query)){
            insert_or_follow_left(current, lower_limit, upper_limit, current_query);
        }
        // If the node's key is smaller to the low part of the query
        // Then follow the right child
        //                  Key
        // Data:  |----------!--------|
        // Query:            |-----|
        //                  low   high
        else if (node_less_equal_query(current, current_query)){
            insert_or_follow_right(current, lower_limit, upper_limit, current_query);
        }
        // If the node's key is inside the query
        // Then follow both children
        //                  Key
        // Data:  |----------!--------|
        // Query:         |-----|
        //               low   high
        else{
            auto query_left = break_query_left(current_query, current);
            auto query_right = break_query_right(current_query, current);
            insert_or_follow_left(current, lower_limit, upper_limit, query_left);
            insert_or_follow_right(current, lower_limit, upper_limit, query_right);
        }
    }
}

QueryShell* CrackingKDTreeNarrow::break_query_left(QueryShell *query, KDNode *node){
    auto query_left = new QueryShell(query);
    for(size_t i = 0; i < query->shell_query->predicate_count(); ++i){
        if(query->shell_query->predicates.at(i).column == node->column){
            query_left->crack_right.at(i) = false;
            query_left->shell_query->predicates.at(i).high = node->key;
        }
    }
    return query_left;
}

QueryShell* CrackingKDTreeNarrow::break_query_right(QueryShell *query, KDNode *node){
    auto query_right = new QueryShell(query);
    for(size_t i = 0; i < query->shell_query->predicate_count(); ++i){
        if(query->shell_query->predicates.at(i).column == node->column){
            query_right->crack_left.at(i) = false;
            query_right->shell_query->predicates.at(i).low = node->key;
        }
    }
    return query_right;
}

// Checks the left child
// If it is null then we reached a partition
// Otherwise, we follow it
void CrackingKDTreeNarrow::insert_or_follow_left(KDNode *current, int64_t lower_limit, int64_t  upper_limit, QueryShell *query){
    if (current->left_child != nullptr)
    {
        nodes_to_check.push_back(current->left_child.get());
        lower_limits.push_back(lower_limit);
        upper_limits.push_back(current->left_position);
        queries.push_back(query);
    }
    else
    {
        current->left_child = insert_new_nodes(query, lower_limit, current->left_position);
    }
}

// Checks the right child
// If it is null then we reached a partition
// Otherwise, we follow it
void CrackingKDTreeNarrow::insert_or_follow_right(KDNode *current, int64_t lower_limit, int64_t upper_limit, QueryShell *query){
    if (current->right_child != nullptr)
    {
        nodes_to_check.push_back(current->right_child.get());
        lower_limits.push_back(current->right_position);
        upper_limits.push_back(upper_limit);
        queries.push_back(query);
    }
    else
    {
        current->right_child = insert_new_nodes(query, current->right_position, upper_limit);
    }
}

// This method inserts the full query in the KD-Tree, creating something like a zig-zag
// with the nodes.
unique_ptr<KDNode> CrackingKDTreeNarrow::insert_new_nodes(QueryShell *query, int64_t lower_limit, int64_t upper_limit){
    // If the partition is smaller than the minimum size then return null
    if(upper_limit - lower_limit < minimum_partition_size)
        return nullptr;


    // first collect all the valid predicates in a vector
    // 0: key
    // 1: column
    // 2: is_high
    std::vector<std::tuple<float, int64_t, bool> > predicates;
    predicates.reserve(query->shell_query->predicate_count() * 2);

    for(int64_t i = 0; i < query->shell_query->predicate_count(); ++i){
        auto predicate = query->shell_query->predicates.at(i);
        if(query->crack_left.at(i))
            predicates.push_back({predicate.low, predicate.column, false});
        if(query->crack_right.at(i))
            predicates.push_back({predicate.high, predicate.column, true});
    }

    if(predicates.size() < 1)
        return nullptr;

    float key;
    int64_t column;
    bool is_high;
    bool current_is_high;

    // Insert the first element 
    // Need to find the first one that can crack
    unique_ptr<KDNode> first;
    int64_t i = 0;
    for(; i < predicates.size(); ++i){
        std::tie(key, column, is_high) = predicates.at(i);
        auto position = table->CrackTable(lower_limit, upper_limit, key, column);
        if(lower_limit < position && position < upper_limit){
            if(position - lower_limit < minimum_partition_size)
                return nullptr;
            first = index->create_node(column, key, position);
            current_is_high = is_high;
            ++i;
            break;
        }
    }


    // Iterate over the rest
    KDNode* current = first.get(); 
    for(; i < predicates.size(); ++i){
        std::tie(key, column, is_high) = predicates.at(i);
        if(current_is_high){
            upper_limit = current->left_position; 
            auto position = table->CrackTable(lower_limit, upper_limit, key, column);
            if(lower_limit < position && position < upper_limit){
                if(position - lower_limit < minimum_partition_size)
                    return first;
                current->left_child = index->create_node(column, key, position);
                current = current->left_child.get();
                current_is_high = is_high;
            }
        }
        else{
            lower_limit = current->right_position; 
            auto position = table->CrackTable(lower_limit, upper_limit, key, column);
            if(lower_limit < position && position < upper_limit){
                if(position - lower_limit < minimum_partition_size)
                    return first;
                current->right_child = index->create_node(column, key, position);
                current = current->right_child.get();
                current_is_high = is_high;
            }
        }
    }
    return first;
}

// Checks if node's column is inside of query
bool CrackingKDTreeNarrow::node_in_query(KDNode *current, QueryShell *shell){
    for(size_t i = 0; i < shell->shell_query->predicate_count(); i++)
    {
        if(current->column == shell->shell_query->predicates.at(i).column)
            return true;
    }
    return false;
}

// If the node's key is greater or equal to the high part of the query
// Then follow the left child
//                  Key
// Data:  |----------!--------|
// Query:      |-----|
//            low   high
bool CrackingKDTreeNarrow::node_greater_equal_query(KDNode *node, QueryShell *query){
    for(size_t i = 0; i < query->shell_query->predicate_count(); i++)
    {
        if(node->column == query->shell_query->predicates.at(i).column){
            auto high = query->shell_query->predicates.at(i).high;
            if(node->key == high){
                query->crack_right.at(i) = false;
            }
            return high <= node->key;
        }
    }
    return false;
}

// If the node's key is smaller to the low part of the query
// Then follow the right child
//                  Key
// Data:  |----------!--------|
// Query:            |-----|
//                  low   high
bool CrackingKDTreeNarrow::node_less_equal_query(KDNode *node, QueryShell *query){
    for(size_t i = 0; i < query->shell_query->predicate_count(); i++)
    {
        if(node->column == query->shell_query->predicates.at(i).column){
            auto low = query->shell_query->predicates.at(i).low;
            if(node->key == low){
                query->crack_left.at(i) = false;
            }
            return node->key <= low;
        }
    }
    return false;
}
