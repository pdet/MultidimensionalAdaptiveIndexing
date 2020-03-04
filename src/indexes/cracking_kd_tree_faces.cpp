#include "kd_node.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree_faces.hpp"
#include <algorithm> // to check if all elements of a vector are true

#define BIT(value, position) (value & ( 1 << position )) >> position
#define BIT_FLIP(value, position) (value ^ ( 1 << position )) 

CrackingKDTreeFaces::CrackingKDTreeFaces(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
CrackingKDTreeFaces::~CrackingKDTreeFaces(){}

void CrackingKDTreeFaces::initialize(Table *table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    index = make_unique<KDTree>(table->row_count());

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
    // ******************
}

void CrackingKDTreeFaces::adapt_index(Query& query){
    // Transform query into points and edges before starting to measure time
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

Table CrackingKDTreeFaces::range_query(Query& query){
    // ******************
    auto start = measurements->time();

    // Search on the index the correct partitions
    auto partitions = index->search(query);

    // Scan the table and returns the row ids 
    auto result = Table(1);
    for (auto partition : partitions)
    {
        auto low = partition.first;
        auto high = partition.second;
        FullScan::scan_partition(table.get(), query, low, high, &result);
    }

    auto end = measurements->time();
    // ******************
    measurements->append(
        "query_time",
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


    //std::cout << (index->get_node_count()) << std::endl;
    //std::cout << (n_tuples_scanned)<<std::endl;
    //std::cout << std::endl;

    return result;
}

void CrackingKDTreeFaces::adapt(Query& query){ 

    if(index->root == nullptr){
        auto position = table->CrackTable(
            0, table->row_count(),
            query.predicates[0].low, 0 
        );
        index->root = index->create_node(0, query.predicates[0].low, position);
    }

    for(size_t dim = 0; dim < query.predicate_count(); ++dim){
        adapt_recursion(
            index->root.get(),
            query,
            dim, query.predicates[dim].low,
            0, table->row_count()
        );
        adapt_recursion(
            index->root.get(),
            query,
            dim, query.predicates[dim].high,
            0, table->row_count()
        );
    }
}

void CrackingKDTreeFaces::adapt_recursion(
    KDNode *current,
    Query& query,
    int64_t pivot_dim,
    float pivot,
    int64_t lower_limit,
    int64_t upper_limit
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
        if(current->key < pivot){
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
            }
        }
    }else{
        //                  Key
        // Data:  |----------!--------|
        // Query:      |-----|
        //            low   high
        if(node_greater_equal_query(current, query)){
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
            }        }
        //                  Key
        // Data:  |----------!--------|
        // Query:            |-----|
        //                  low   high
        else if(node_less_equal_query(current, query)){
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
        }
        //                  Key
        // Data:  |----------!--------|
        // Query:         |-----|
        //               low   high
        else{
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
        }
    } 
}

// If the node's key is greater or equal to the high part of the query
// Then follow the left child
//                  Key
// Data:  |----------!--------|
// Query:      |-----|
//            low   high
bool CrackingKDTreeFaces::node_greater_equal_query(KDNode *node, Query& query){
    for(int64_t i = 0; i < query.predicate_count(); i++)
    {
        if(node->column == query.predicates[i].column){
            auto high = query.predicates[i].high;
            return high <= node->key;
        }
    }
    assert(false);
    return false;
}

// If the node's key is smaller to the low part of the query
// Then follow the right child
//                  Key
// Data:  |----------!--------|
// Query:            |-----|
//                  low   high
bool CrackingKDTreeFaces::node_less_equal_query(KDNode *node, Query& query){
    for(int64_t i = 0; i < query.predicate_count(); i++)
    {
        if(node->column == query.predicates[i].column){
            auto low = query.predicates[i].low;
            return node->key <= low;
        }
    }
    assert(false);
    return false;
}
