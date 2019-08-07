#include "standard_cracking.hpp"
#include <algorithm>
#include <math.h>
#include <limits>
#include "full_scan.hpp"

StandardCracking::StandardCracking(){}
StandardCracking::~StandardCracking(){} 

void StandardCracking::initialize(const shared_ptr<Table> table_to_copy){
    auto start = measurements->time();

    // Copy the table pointer to reconstruct tuples afterwards
    table = table_to_copy;

    // Splits table's attributes and add an ID column in each new table to reconstruct afterwards.
    std::vector<std::vector<float>> ids(1, std::vector<float>(table->row_count()));
    std::iota(std::begin(ids.at(0)), std::end(ids.at(0)), 0); 
    for(auto &col : table_to_copy->columns){
        Table t = Table(ids);
        t.append_column(*col);
        cracker_columns.push_back(t);         
    }
   
    // Create all the index for each attribute
    index.resize(table->col_count()); 
    for(auto &i : index){
        i.insert(StandardCrackingNode(
                    std::numeric_limits<float>::lowest(),
                    0
        ));
        i.insert(StandardCrackingNode(
                    std::numeric_limits<float>::infinity(),
                    table->row_count()
        ));
    }
    auto end = measurements->time();

    measurements->initialization_time = Measurements::difference(end, start);
}

void StandardCracking::adapt_index(Query& query){
    // ******************
    auto start = measurements->time();
    // Adapt the indexes
    adapt(query);
    auto end = measurements->time();
    // ******************
    measurements->adaptation_time.push_back(
        Measurements::difference(end, start)
    );

}

shared_ptr<Table> StandardCracking::range_query(Query& query){
    // ******************
    auto start = measurements->time();

    // Search on the index the correct rows
    auto bitvector = search(query);

    // Scan the table and returns a materialized view of the result.
    auto result = make_shared<Table>(table->col_count());
    for (size_t i = 0; i < bitvector.size(); ++i)
    {
        auto bit = bitvector.at(i);
        if(bit){
            result->append(
                    table->materialize_row(i)
                    );
        }
    }

    auto end = measurements->time();
    // ******************
    measurements->query_time.push_back(
        Measurements::difference(end, start)
    );

    size_t n_tuples_scanned = 0;
    for(auto bit: bitvector){
        if(bit)
            n_tuples_scanned += 1;
     }

    // Before returning the result, update the statistics.
    measurements->number_of_nodes.push_back(get_node_count());
    measurements->max_height.push_back(floor(log2(smallest_index())));
    measurements->min_height.push_back(floor(log2(biggest_index())));
    measurements->memory_footprint.push_back(get_node_count() * sizeof(StandardCrackingNode));
    measurements->tuples_scanned.push_back(n_tuples_scanned);

    return result;

}


void StandardCracking::adapt(Query &query){
// Iterate each predicate
//  get index from dimension
//  search lower_bound of low and high
//  if both are in same partition Crack in Three
//  else crack in two each
}

std::vector<char> StandardCracking::search(Query &query){
// Create a bitvector of chars
// For each query
//  search the lower_bounds of low and high
//  Scan the partition and add to bitvector
// Return bitvector
}

size_t StandardCracking::get_node_count(){
    size_t node_count = 0;
    for(auto &i : index)
        node_count += i.size();
    return node_count;
}

size_t StandardCracking::biggest_index(){
    size_t biggest = 0;
    for(auto &i: index){
        if(i.size() > biggest)
            biggest = i.size();
    }
    return biggest;
}

size_t StandardCracking::smallest_index(){
    size_t smallest = std::numeric_limits<size_t>::max();
    for(auto &i : index){
        if(i.size() < smallest)
            smallest = i.size();
    }
    return smallest;
}
