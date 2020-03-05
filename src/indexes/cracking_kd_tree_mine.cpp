#include "kd_node.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree_mine.hpp"
#include <algorithm> // to check if all elements of a vector are true

#define BIT(value, position) (value & ( 1 << position )) >> position
#define BIT_FLIP(value, position) (value ^ ( 1 << position )) 

CrackingKDTreeMine::CrackingKDTreeMine(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}
CrackingKDTreeMine::~CrackingKDTreeMine(){}

void CrackingKDTreeMine::initialize(Table *table_to_copy){
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

void CrackingKDTreeMine::adapt_index(Query& query){
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

Table CrackingKDTreeMine::range_query(Query& query){
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

void CrackingKDTreeMine::adapt(
    std::vector<Point> &points,
    std::vector<Edge> &edges
    ){ 
    auto start = measurements->time();
    // Insert points
    for(auto i = 0; i < points.size(); ++i){
        insert_point(points.at(i), i);
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

void CrackingKDTreeMine::insert_point(
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
            point.at(0), 0 
        );
        index->root = index->create_node(0, point.at(0), position);
        should_insert.at(0) = false;
        current = index->root.get();
    }

    while(true){
        // If equal
        if(current->key == point.at(current->column)){
            should_insert.at(current->column) = false;
            //If is right hand side of query then we follow left
            if(BIT(is_right_hand_side, current->column)){
                if(current->left_child == nullptr){
                    crack_point(
                            point, is_right_hand_side, should_insert,
                            current, low_position, current->position 
                            );
                    return;
                }
                current = current->left_child.get();
                high_position = current->position;
            }
            //If is left hand side of query then we follow right
            else{
                if(current->right_child == nullptr){
                    crack_point(
                            point, is_right_hand_side, should_insert,
                            current, current->position, high_position
                            );
                    return;
                }
                current = current->right_child.get();
                low_position = current->position;
            }
        }
        // follow right
        else if(current->key < point.at(current->column)){
            if(current->right_child == nullptr){
                crack_point(
                    point, is_right_hand_side, should_insert,
                    current, current->position, high_position
                );
                return;
            }
            current = current->right_child.get();
            low_position = current->position;
        }
        // follow left
        else{
            if(current->left_child == nullptr){
                crack_point(
                    point, is_right_hand_side, should_insert,
                    current, low_position, current->position 
                );
                return;
            }
            current = current->left_child.get();
            high_position = current->position;
        }
    }

}


void CrackingKDTreeMine::insert_edge(CrackingKDTreeMine::Edge& edge){
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

    // Iterate over the tree to find the partitions where to insert the edge
    MyStack<KDNode*> nodes_to_check;
    MyStack<int64_t> lower_limits;
    MyStack<int64_t> upper_limits;
    nodes_to_check.push_back(index->root.get());
    lower_limits.push_back(0);
    upper_limits.push_back(table->row_count());
    while(!nodes_to_check.empty()){
        auto current = nodes_to_check.pop_back();

        auto lower_limit = lower_limits.pop_back();

        auto upper_limit = upper_limits.pop_back();

        // If the size of the partition is already too small then stop exploring it
        if(upper_limit - lower_limit + 1 < minimum_partition_size)
            continue;

        // If that pivot has already been inserted then we don't need to
        // keep looking in this branch.
        if(current->column == pivot_dim && current->key == edge.first.at(pivot_dim)){
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
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->left_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
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
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->right_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
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
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->left_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
                        );
                continue;
            }
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->position);
            nodes_to_check.push_back(current->left_child.get());

            if(current->right_child == nullptr){
                auto position = table->CrackTable(
                        current->position, upper_limit,
                        edge.first.at(pivot_dim), pivot_dim 
                        );
                current->right_child = index->create_node(
                        pivot_dim, edge.first.at(pivot_dim), position
                        );
                continue;
            }
            lower_limits.push_back(current->position);
            upper_limits.push_back(upper_limit);
            nodes_to_check.push_back(current->right_child.get());
        }

    }

}

float CrackingKDTreeMine::max(CrackingKDTreeMine::Point &p1, CrackingKDTreeMine::Point &p2, size_t dimension){
    if(p1.at(dimension) > p2.at(dimension))
        return p1.at(dimension);
    return p2.at(dimension);
}

float CrackingKDTreeMine::min(CrackingKDTreeMine::Point &p1, CrackingKDTreeMine::Point &p2, size_t dimension){
    if(p1.at(dimension) < p2.at(dimension))
        return p1.at(dimension);
    return p2.at(dimension);
}

void CrackingKDTreeMine::crack_point(
    Point &point,   // point to be inserted
    size_t is_right_hand_side,  // if each axis of the point comes
                                //  from the right side of query
    std::vector<bool> &should_insert, // if the axis should be inserted
    KDNode* current, // node to insert the new point
    int64_t low_position,       // lower position from partition
    int64_t high_position      // upper position from partition
    ){

    // if there is no dimensions to insert, then return 
    if(all_elements_false(should_insert))
        return;

    // if the minimum threshold for partition size exceded, then return
    if(high_position - low_position < minimum_partition_size)
        return;

    auto dimension = next_dim(current->column, should_insert);

    // insert the first possible dimension
    if(should_insert.at(current->column)){
        if(current->key <= point.at(current->column)){
            auto position = table->CrackTable(
                current->position, high_position,
                point.at(dimension), dimension 
            );
            current->right_child = index->create_node(
                dimension, point.at(dimension), position
            );

            low_position = current->position;
            current = current->right_child.get();
        }else{
            auto position = table->CrackTable(
                low_position, current->position,
                point.at(dimension), dimension 
            );
            current->left_child = index->create_node(
                dimension, point.at(dimension), position
            );

            high_position = current->position;
            current = current->left_child.get();
        }
    should_insert.at(dimension) = false;
    }

    // while we still have dimensions to insert
    while(!all_elements_false(should_insert)){

        if(high_position - low_position < minimum_partition_size)
            return;

        dimension = next_dim(dimension, should_insert);
        // If the dimension of the current node came from the right side of a predicate
        // Then we need to insert to the left side of the current node
        if(BIT(is_right_hand_side, current->column)){
            auto position = table->CrackTable(
                    low_position, current->position,
                    point.at(dimension), dimension 
                    );
            current->left_child = index->create_node(
                    dimension, point.at(dimension), position
                    );

            high_position = current->position;
            current = current->left_child.get();
        }
        // If the dimension of the current node came from the left side of a predicate
        // Then we need to insert to the right side of the current node
        else{
            auto position = table->CrackTable(
                    current->position, high_position,
                    point.at(dimension), dimension 
                    );
            current->right_child = index->create_node(
                    dimension, point.at(dimension), position
                    );

            low_position = current->position;
            current = current->right_child.get();
        }

        should_insert.at(dimension) = false;
    }
}

// Finds the next dimension that should be inserted
int64_t CrackingKDTreeMine::next_dim(int64_t start, std::vector<bool> &should_insert){
    auto n_dimensions = should_insert.size();
    int64_t next = (start + 1) % n_dimensions;
    while(next != start){
        if(should_insert.at(next))
            return next;
        next = (next + 1) % n_dimensions;
    }
    // If next == start then there is no next dimension 
    return next;
}


bool CrackingKDTreeMine::all_elements_false(std::vector<bool> &v){
    return std::all_of(
        v.begin(), v.end(), [](bool i){return !i;}
    );
}

std::vector<CrackingKDTreeMine::Point> CrackingKDTreeMine::query_to_points(Query& query){
    auto number_of_dimensions = query.predicate_count();
    auto number_of_points = 2 << (number_of_dimensions - 1); 
    std::vector<Point> points(number_of_points);

    for(auto i = 0; i < number_of_points; ++i){
        Point point(number_of_dimensions);
        for(auto j = 0; j < number_of_dimensions; ++j){
            // access the j-bit of i
            if(BIT(i, j))
                point.at(j) = query.predicates.at(j).high;
            else
                point.at(j) = query.predicates.at(j).low;
        }
        points.at(i) = point;
    }

    return points;
}

std::vector<CrackingKDTreeMine::Edge> CrackingKDTreeMine::query_to_edges(Query& query){
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

CrackingKDTreeMine::Point CrackingKDTreeMine::decompress_edge(size_t compressed_edge, Query& query){
    auto number_of_dimensions = query.predicate_count();
    Point point(number_of_dimensions);
    for(auto j = 0; j < number_of_dimensions; ++j){
        // access the j-bit of i
        if(BIT(compressed_edge, j))
            point.at(j) = query.predicates.at(j).high;
        else
            point.at(j) = query.predicates.at(j).low;
    }
    return point;
}
