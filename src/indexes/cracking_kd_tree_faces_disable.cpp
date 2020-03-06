#include "kd_node.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree_faces_disable.hpp"
#include <algorithm> // to check if all elements of a vector are true

CrackingKDTreeFacesDisable::CrackingKDTreeFacesDisable(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);

    if(config.find("adaptation_parameter") == config.end())
        adaptation_parameter = 0.05;
    else
        adaptation_parameter = std::stof(config["adaptation_parameter"]);
}
CrackingKDTreeFacesDisable::~CrackingKDTreeFacesDisable(){}

void CrackingKDTreeFacesDisable::initialize(Table *table_to_copy){
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

void CrackingKDTreeFacesDisable::adapt_index(Query& query){
    // Transform query into points and edges before starting to measure time
    // Adapt the KDTree 
    auto old_node_count = index->get_node_count();
    auto start = measurements->time();
    if(should_adapt)
        adapt(query);
    auto end = measurements->time();
    // ******************
    measurements->append(
            "adaptation_time",
            std::to_string(Measurements::difference(end, start))
            );
    auto new_node_count = index->get_node_count();
    if(should_adapt && (new_node_count - old_node_count) < old_node_count * adaptation_parameter){
        should_adapt=false;
    }

    measurements->append("number_of_nodes", std::to_string(index->get_node_count()));
    measurements->append("max_height", std::to_string(index->get_max_height()));
    measurements->append("min_height", std::to_string(index->get_min_height()));
    measurements->append("memory_footprint", std::to_string(index->get_node_count() * sizeof(KDNode)));


}

Table CrackingKDTreeFacesDisable::range_query(Query& query){
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

void CrackingKDTreeFacesDisable::adapt(Query& query){ 

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

void CrackingKDTreeFacesDisable::adapt_recursion(
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
        if(pivot < current->key){
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
                return;
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
                return;
            }
        }
    }else{
        switch(current->compare(query)){
            case -1:
                //                  Key
                // Data:  |----------!--------|
                // Query:            |-----|
                //                  low   high
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
                    return;
                }
                break;
            case +1:
                //                  Key
                // Data:  |----------!--------|
                // Query:      |-----|
                //            low   high
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
                    return;
                }
                break;
            case 0:
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
                break;
            default:
                assert(false);
                break;
        } 
    }
}
