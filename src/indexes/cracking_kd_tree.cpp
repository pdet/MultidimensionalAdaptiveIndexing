#include "kd_node.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree.hpp"
#include <algorithm> // to check if all elements of a vector are true

#define BIT(value, position) (value & ( 1 << position )) >> position
#define BIT_FLIP(value, position) (value ^ ( 1 << position )) 

CrackingKDTree::CrackingKDTree(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
CrackingKDTree::~CrackingKDTree(){}

void CrackingKDTree::initialize(Table *table_to_copy){
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

void CrackingKDTree::adapt_index(Query& query){
    // Transform query into points and edges before starting to measure time
    auto start = measurements->time();
    auto points = query_to_points(query);
    auto end = measurements->time();
    measurements->append(
        "point_generation",
        std::to_string(Measurements::difference(end, start))
    );

    start = measurements->time();
    auto edges = query_to_edges(query); 
    end = measurements->time();
    // ******************
    measurements->append(
        "edge_generation",
        std::to_string(Measurements::difference(end, start))
    );

    start = measurements->time();
    // Adapt the KDTree 
    adapt(points, edges);
    end = measurements->time();
    // ******************
    measurements->append(
        "adaptation_time",
        std::to_string(Measurements::difference(end, start))
    );

}

Table CrackingKDTree::range_query(Query& query){
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

void CrackingKDTree::adapt(
    std::vector<Point> &points,
    std::vector<Edge> &edges
    ){ 
    auto start = measurements->time();
    // Insert points
    for(auto i = 0; i < points.size(); ++i){
        insert_point(points[i], i);
    }
    auto end = measurements->time();
    measurements->append(
        "insert_points",
        std::to_string(Measurements::difference(end, start))
    );

    start = measurements->time();
    // Insert hyperplanes
    for(auto& edge : edges){
      insert_edge(edge);
    }
    end = measurements->time();
    measurements->append(
        "insert_edges",
        std::to_string(Measurements::difference(end, start))
    );
}

void CrackingKDTree::insert_point(
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


void CrackingKDTree::insert_edge(CrackingKDTree::Edge& edge){
    // Determine which dimension value is fixed
    // (x1, y1) - (x2, y1)
    // Here y1 is fixed, which means this will be the pivot
    
    auto n_dimensions = edge.first.size();
    size_t pivot_dim = 0;
    for(size_t i = 0; i < n_dimensions; ++i){
        if(edge.first[i] == edge.second[i]){
            pivot_dim = i;
            break;
        }
    }

    // Iterate over the tree to find the partitions where to insert the edge
    std::vector<KDNode*> nodes_to_check;
    std::vector<int64_t> lower_limits;
    std::vector<int64_t> upper_limits;
    nodes_to_check.push_back(index->root.get());
    lower_limits.push_back(0);
    upper_limits.push_back(table->row_count());
    while(!nodes_to_check.empty()){
        auto current = nodes_to_check.back();
        nodes_to_check.pop_back();

        auto lower_limit = lower_limits.back();
        lower_limits.pop_back();

        auto upper_limit = upper_limits.back();
        upper_limits.pop_back();

        // If the size of the partition is already too small then stop exploring it
        if(upper_limit - lower_limit + 1 < minimum_partition_size)
            continue;

        // If that pivot has already been inserted then we don't need to
        // keep looking in this branch.
        if(current->column == pivot_dim && current->key == edge.first[pivot_dim]){
            continue;
        }

        //                  Key
        // Data:  |----------!--------|
        // Query:      |-----|
        //            low   high
        if(max(edge.first, edge.second, current->column) <= current->key){
            if(current->left_child == nullptr){
                auto position = table->CrackTable(
                        lower_limit, current->position,
                        edge.first[pivot_dim], pivot_dim 
                        );
                current->left_child = index->create_node(
                        pivot_dim, edge.first[pivot_dim], position
                        );
                continue;
            }
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->position);
            nodes_to_check.push_back(current->left_child.get());
        }
        //                  Key
        // Data:  |----------!--------|
        // Query:            |-----|
        //                  low   high
        else if(current->key <= min(edge.first, edge.second, current->column)){
            if(current->right_child == nullptr){
                auto position = table->CrackTable(
                        current->position, upper_limit,
                        edge.first[pivot_dim], pivot_dim 
                        );
                current->right_child = index->create_node(
                        pivot_dim, edge.first[pivot_dim], position
                        );
                continue;
            }
            lower_limits.push_back(current->position);
            upper_limits.push_back(upper_limit);
            nodes_to_check.push_back(current->right_child.get());

        }
        //                  Key
        // Data:  |----------!--------|
        // Query:         |-----|
        //               low   high
        else{
            if(current->left_child == nullptr){
                auto position = table->CrackTable(
                        lower_limit, current->position,
                        edge.first[pivot_dim], pivot_dim 
                        );
                current->left_child = index->create_node(
                        pivot_dim, edge.first[pivot_dim], position
                        );
                continue;
            }
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->position);
            nodes_to_check.push_back(current->left_child.get());

            if(current->right_child == nullptr){
                auto position = table->CrackTable(
                        current->position, upper_limit,
                        edge.first[pivot_dim], pivot_dim 
                        );
                current->right_child = index->create_node(
                        pivot_dim, edge.first[pivot_dim], position
                        );
                continue;
            }
            lower_limits.push_back(current->position);
            upper_limits.push_back(upper_limit);
            nodes_to_check.push_back(current->right_child.get());
        }

    }

}

float CrackingKDTree::max(CrackingKDTree::Point &p1, CrackingKDTree::Point &p2, size_t dimension){
    if(p1[dimension] > p2[dimension])
        return p1[dimension];
    return p2[dimension];
}

float CrackingKDTree::min(CrackingKDTree::Point &p1, CrackingKDTree::Point &p2, size_t dimension){
    if(p1[dimension] < p2[dimension])
        return p1[dimension];
    return p2[dimension];
}

// Finds the next dimension that should be inserted
int64_t CrackingKDTree::next_dim(int64_t start, std::vector<bool> &should_insert){
    auto n_dimensions = should_insert.size();
    int64_t next = (start + 1) % n_dimensions;
    while(next != start){
        if(should_insert[next])
            return next;
        next = (next + 1) % n_dimensions;
    }
    // If next == start then there is no next dimension 
    return next;
}


bool CrackingKDTree::all_elements_false(std::vector<bool> &v){
    return std::all_of(
        v.begin(), v.end(), [](bool i){return !i;}
    );
}

std::vector<CrackingKDTree::Point> CrackingKDTree::query_to_points(Query& query){
    auto number_of_dimensions = query.predicate_count();
    auto number_of_points = 2 << (number_of_dimensions - 1); 
    std::vector<Point> points(number_of_points);

    for(auto i = 0; i < number_of_points; ++i){
        Point point(number_of_dimensions);
        for(auto j = 0; j < number_of_dimensions; ++j){
            // access the j-bit of i
            if(BIT(i, j))
                point[j] = query.predicates[j].high;
            else
                point[j] = query.predicates[j].low;
        }
        points[i] = point;
    }

    return points;
}

std::vector<CrackingKDTree::Edge> CrackingKDTree::query_to_edges(Query& query){
    auto number_of_dimensions = query.predicate_count();
    auto number_of_points = 2 << (number_of_dimensions - 1);

    // Monstrosity to compare the compressed edges in the set.
    // Simply using a set and the algorithm below will yield edges like:
    // x1,y1 -- x1,y2
    // x1,y2 -- x1,y1
    // We want to avoid the duplication of edges
    auto comp = [] (const std::pair<size_t, size_t>& p1, const std::pair<size_t, size_t>& p2) -> bool{
        return  !((p1.first == p2.first && p1.second == p2.second) ||
                (p1.first == p2.second && p1.second == p2.first));

    };
    std::set<std::pair<size_t, size_t>, decltype(comp) > compressed_edges (comp);
    for(size_t p = 0; p < number_of_points; ++p){
        for(size_t i = 0; i < number_of_dimensions; ++i){
            compressed_edges.insert(std::make_pair(p, BIT_FLIP(p, i)));
        }
    }

    assert(compressed_edges.size() == number_of_dimensions * (2 << (number_of_dimensions - 2)));

    // Now transform the compressed edges to actual points
    std::vector<Edge> edges;

    for(auto& pair : compressed_edges){
        edges.push_back(
            Edge(
                decompress_edge(pair.first, query),
                decompress_edge(pair.second, query)
                )
        );
    }

    //for(auto& predicate : query.predicates){
    //    std::cout << predicate.low << " <= ";
    //    std::cout << predicate.column << " < ";
    //    std::cout << predicate.high << " AND ";
    //}
    //std::cout << std::endl;

    //for(auto& edge : edges){
    //   for(auto &v : edge.first)
    //        std::cout << v << ", ";
    //   std::cout << " --> ";

    //   for(auto &v : edge.second)
    //        std::cout << v << ", ";
    //   std::cout << std::endl;

    //}

    return edges;
}

CrackingKDTree::Point CrackingKDTree::decompress_edge(size_t compressed_edge, Query& query){
    auto number_of_dimensions = query.predicate_count();
    Point point(number_of_dimensions);
    for(auto j = 0; j < number_of_dimensions; ++j){
        // access the j-bit of i
        if(BIT(compressed_edge, j))
            point[j] = query.predicates[j].high;
        else
            point[j] = query.predicates[j].low;
    }
    return point;
}
