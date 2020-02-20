#include "kd_node.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree.hpp"
#include<algorithm> // to check if all elements of a vector are true

#define BIT(value, position) (value & ( 1 << position )) >> position

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

    measurements->initialization_time = Measurements::difference(end, start);
    // ******************
}

void CrackingKDTree::adapt_index(Query& query){
    // ******************
    auto start = measurements->time();
    // Adapt the KDTree 
    adapt(query);
    auto end = measurements->time();
    // ******************
    measurements->adaptation_time.push_back(
        Measurements::difference(end, start)
    );
}

Table CrackingKDTree::range_query(Query& query){
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


    //std::cout << (index->get_node_count()) << std::endl;
    //std::cout << (n_tuples_scanned)<<std::endl;
    //std::cout << std::endl;


    return result;
}

void CrackingKDTree::adapt(Query& query){
    // Transform query into points
    auto points = query_to_points(query);
    // Insert all points
    //for(auto& predicate : query.predicates){
    //    std::cout << predicate.low << "<=" << predicate.column << "<" << predicate.high << " AND ";
    //}
    //std::cout << std::endl;
    for(auto i = 0; i < points.size(); ++i){
        //std::cout << i << ": ";
        //for(auto& v : points.at(i))
        //    std::cout << v << " ";
        //std::cout << std::endl;
        insert_point(points.at(i), i);
    }

    // Get all hyperplanes
    // Insert hyperplanes
}

void CrackingKDTree::insert_point(
    std::vector<float> point,
    size_t is_right_hand_side
){
    std::vector<bool> should_insert(point.size(), true);

    KDNode* current = index->root.get();
    int64_t low_position = 0;
    int64_t high_position = table->row_count() - 1;

    if(current == nullptr){
        // Add new root
        auto position = table->CrackTable(
            low_position, high_position,
            point.at(0), 0 
        );
        index->root = index->create_node(0, point.at(0), position - 1);
        should_insert.at(0) = false;
        current = index->root.get();
    }

    while(true){
        // follow right
        if(point.at(current->column) >= current->key){
            if(current->right_child == nullptr){
                crack_point(
                    point, is_right_hand_side, should_insert,
                    current, current->right_position, high_position
                );
                return;
            }
            current = current->right_child.get();
            low_position = current->right_position;
        }
        // follow left
        else{
            crack_point(
                    point, is_right_hand_side, should_insert,
                    current, low_position, current->left_position 
                    );
            return;
            current = current->left_child.get();
            high_position = current->left_position;
        }
    }

}

void CrackingKDTree::crack_point(
    std::vector<float> point,   // point to be inserted
    size_t is_right_hand_side,  // if each axis of the point comes
                                //  from the right side of query
    std::vector<bool> should_insert, // if the axis should be inserted
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
    if(current->key < point.at(current->column)){
        auto position = table->CrackTable(
            current->right_position, high_position,
            point.at(dimension), dimension 
        );
        current->right_child = index->create_node(
            dimension, point.at(dimension), position - 1
        );

        low_position = current->right_position;
        current = current->right_child.get();
    }else{
        auto position = table->CrackTable(
            low_position, current->left_position,
            point.at(dimension), dimension 
        );
        current->left_child = index->create_node(
            dimension, point.at(dimension), position - 1
        );

        high_position = current->left_position;
        current = current->left_child.get();
    }

    should_insert.at(dimension) = false;

    // while we still have dimensions to insert
    while(!all_elements_false(should_insert)){

        if(high_position - low_position < minimum_partition_size)
            return;

        dimension = next_dim(dimension, should_insert);
        // If the current dimension of the point came from the right side of a predicate
        // Then we need to insert to the left side of the current node
        if(BIT(is_right_hand_side, dimension)){
            auto position = table->CrackTable(
                    low_position, current->left_position,
                    point.at(dimension), dimension 
                    );
            current->left_child = index->create_node(
                    dimension, point.at(dimension), position - 1
                    );

            high_position = current->left_position;
            current = current->left_child.get();
        }
        // If the current dimension of the point came from the left side of a predicate
        // Then we need to insert to the right side of the current node
        else{
            auto position = table->CrackTable(
                    current->right_position, high_position,
                    point.at(dimension), dimension 
                    );
            current->right_child = index->create_node(
                    dimension, point.at(dimension), position - 1
                    );

            low_position = current->right_position;
            current = current->right_child.get();
        }

        should_insert.at(dimension) = false;
    }
}

// Finds the next dimension that should be inserted
int64_t CrackingKDTree::next_dim(int64_t start, std::vector<bool> should_insert){
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


bool CrackingKDTree::all_elements_false(std::vector<bool> v){
    return std::all_of(
        v.begin(), v.end(), [](bool i){return !i;}
    );
}

std::vector<std::vector<float> > CrackingKDTree::query_to_points(Query& query){
    auto number_of_dimensions = query.predicate_count();
    auto number_of_points = 2 << (number_of_dimensions - 1); 
    std::vector<std::vector<float> > points(number_of_points);

    for(auto i = 0; i < number_of_points; ++i){
        std::vector<float> point(number_of_dimensions);
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
