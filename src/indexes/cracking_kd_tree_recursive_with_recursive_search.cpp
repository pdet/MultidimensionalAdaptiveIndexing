#include "kd_node.hpp"
#include "cracking_kd_tree_recursive_with_recursive_search.hpp"

#define BIT(value, position) (value & ( 1 << position )) >> position
#define BIT_FLIP(value, position) (value ^ ( 1 << position )) 

CrackingKDTreeRecursiveWithRecursiveSearch::CrackingKDTreeRecursiveWithRecursiveSearch(std::map<std::string, std::string> config)
: CrackingKDTree(config){}
CrackingKDTreeRecursiveWithRecursiveSearch::~CrackingKDTreeRecursiveWithRecursiveSearch(){}

void CrackingKDTreeRecursiveWithRecursiveSearch::initialize(Table *table_to_copy){
    // ******************
    auto start = measurements->time();

    // Copy the entire table
    table = make_unique<Table>(table_to_copy);

    index = make_unique<KDTreeRecursive>(table->row_count());

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
    // ******************
}

Table CrackingKDTreeRecursiveWithRecursiveSearch::range_query(Query& query){
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
    measurements->append(
        "index_efficiency",
        std::to_string(
            result.row_count()/static_cast<float>(n_tuples_scanned)
        )
    );

    //std::cout << (index->get_node_count()) << std::endl;
    //std::cout << (n_tuples_scanned)<<std::endl;
    //std::cout << std::endl;

    return result;
}
void CrackingKDTreeRecursiveWithRecursiveSearch::insert_point(
        Point &point,
        size_t is_right_hand_side
        ){
    std::vector<bool> should_insert(point.size(), true);

    KDNode* current = index->root.get();
    int64_t low_position = 0;
    int64_t high_position = table->row_count();

    if(current == nullptr){
        // Add new root
        auto position = table->CrackTable(
                low_position, high_position,
                point[0], 0 
                );
        index->root = index->create_node(0, point[0], position);
        should_insert[0] = false;
        current = index->root.get();
    }

    while(!all_elements_false(should_insert)){
        // if the minimum threshold for partition size exceded, then return
        if(high_position - low_position < minimum_partition_size)
            return;

        // If equal
        if(current->key == point[current->column]){
            should_insert[current->column] = false;
            //If is right hand side of query then we follow left
            if(BIT(is_right_hand_side, current->column)){
                if(current->left_child == nullptr){
                    auto next_dimension = next_dim(current->column, should_insert);
                    if(should_insert[next_dimension]){
                        auto position = table->CrackTable(
                                low_position, current->position,
                                point[next_dimension], next_dimension
                                );
                        current->left_child = index->create_node(
                                next_dimension, point[next_dimension], position
                                );
                        should_insert[next_dimension] = false;
                    }
                }
                high_position = current->position;
                current = current->left_child.get();
            }
            //If is left hand side of query then we follow right
            else{
                if(current->right_child == nullptr){
                    auto next_dimension = next_dim(current->column, should_insert);
                    if(should_insert[next_dimension]){
                        auto position = table->CrackTable(
                                current->position, high_position,
                                point[next_dimension], next_dimension
                                );
                        current->right_child = index->create_node(
                                next_dimension, point[next_dimension], position
                                );
                        should_insert[next_dimension] = false;
                    }
                }
                low_position = current->position;
                current = current->right_child.get();
            }
        }
        // if point greater than current node follow right
        else if(current->key < point[current->column]){
            if(current->right_child == nullptr){
                auto next_dimension = next_dim(current->column, should_insert);
                if(should_insert[next_dimension]){
                    auto position = table->CrackTable(
                            current->position, high_position,
                            point[next_dimension], next_dimension
                            );
                    current->right_child = index->create_node(
                            next_dimension, point[next_dimension], position
                            );
                    should_insert[next_dimension] = false;
                }
            }
            low_position = current->position;
            current = current->right_child.get();
        }
        // if point smaller than current node follow left
        else{
            if(current->left_child == nullptr){
                auto next_dimension = next_dim(current->column, should_insert);
                if(should_insert[next_dimension]){
                auto position = table->CrackTable(
                        low_position, current->position,
                        point[next_dimension], next_dimension
                        );
                current->left_child = index->create_node(
                        next_dimension, point[next_dimension], position
                        );
                should_insert[next_dimension] = false;
                }
            }
            high_position = current->position;
            current = current->left_child.get();
        }
    }

}


void CrackingKDTreeRecursiveWithRecursiveSearch::insert_edge_recursion(
    KDNode *current,
    CrackingKDTreeRecursiveWithRecursiveSearch::Edge& edge,
    int64_t lower_limit,
    int64_t upper_limit,
    size_t pivot_dim
){
    // If the size of the partition is already too small then stop exploring it
    if(upper_limit - lower_limit + 1 < minimum_partition_size)
        return;

    // If that pivot has already been inserted then we don't need to
    // keep looking in this branch.
    if(current->column == pivot_dim && current->key == edge.first.at(pivot_dim)){
        return;
    }

    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    if(max(edge.first, edge.second, current->column) <= current->key){
        if(current->left_child == nullptr){
            auto position = table->CrackTable(
                    lower_limit, current->position,
                    edge.first.at(pivot_dim), pivot_dim 
                    );
            current->left_child = index->create_node(
                    pivot_dim, edge.first.at(pivot_dim), position
                    );
            return;
        }
        insert_edge_recursion(
            current->left_child.get(),
            edge, lower_limit, current->position, pivot_dim
        );
    }
    //                  Key
    // Data:  |----------!--------|
    // Query:            |-----|
    //                  low   high
    else if(current->key <= min(edge.first, edge.second, current->column)){
        if(current->right_child == nullptr){
            auto position = table->CrackTable(
                    current->position, upper_limit,
                    edge.first.at(pivot_dim), pivot_dim 
                    );
            current->right_child = index->create_node(
                    pivot_dim, edge.first.at(pivot_dim), position
                    );
            return;
        }
        insert_edge_recursion(
            current->right_child.get(),
            edge, current->position, upper_limit, pivot_dim
        );
    }
    //                  Key
    // Data:  |----------!--------|
    // Query:         |-----|
    //               low   high
    else{
        if(current->left_child == nullptr){
            auto position = table->CrackTable(
                    lower_limit, current->position,
                    edge.first.at(pivot_dim), pivot_dim 
                    );
            current->left_child = index->create_node(
                    pivot_dim, edge.first.at(pivot_dim), position
                    );
            return;
        }
        insert_edge_recursion(
            current->left_child.get(),
            edge, lower_limit, current->position, pivot_dim
        );

        if(current->right_child == nullptr){
            auto position = table->CrackTable(
                    current->position, upper_limit,
                    edge.first.at(pivot_dim), pivot_dim 
                    );
            current->right_child = index->create_node(
                    pivot_dim, edge.first.at(pivot_dim), position
                    );
            return;
        }
        insert_edge_recursion(
            current->right_child.get(),
            edge, current->position, upper_limit, pivot_dim
        );
    }

}

void CrackingKDTreeRecursiveWithRecursiveSearch::insert_edge(CrackingKDTree::Edge& edge){
    // Determine which dimension value is fixed
    // (x1, y1) - (x2, y1)
    // Here y1 is fixed, which means this will be the pivot
    auto n_dimensions = edge.first.size();
    size_t pivot_dim = 0;
    for(size_t i = 0; i < n_dimensions; ++i){
        if(edge.first.at(i) == edge.second.at(i)){
            pivot_dim = i;
            break;
        }
    }
    insert_edge_recursion(index->root.get(), edge, 0, table->row_count(), pivot_dim);
}
