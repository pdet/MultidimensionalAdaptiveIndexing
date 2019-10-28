#include "standard_cracking.hpp"
#include <algorithm>
#include <math.h>
#include <limits>
#include "full_scan.hpp"
#include <iostream>

StandardCracking::StandardCracking(std::map<std::string, std::string> config){
    if(config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);

}
StandardCracking::~StandardCracking(){} 

void StandardCracking::initialize(Table* table_to_copy){
    
    auto start = measurements->time();

    // Copy the table pointer to reconstruct tuples afterwards
    table = make_unique<Table>(table_to_copy);

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
                    table->row_count() - 1
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

Table StandardCracking::range_query(Query& query){
    // ******************
    auto start = measurements->time();

    // Search on the index the correct rows
    auto bitvector = search(query);

    // Scan the table and returns a materialized view of the result.
    auto result = Table(table->col_count());
    for (int64_t i = 0; i < bitvector.size(); ++i)
    {
        auto bit = bitvector.at(i);
        if(bit == '1'){
            result.append(
                    table->materialize_row(i)
                    );
        }
    }

    auto end = measurements->time();
    // ******************
    measurements->query_time.push_back(
        Measurements::difference(end, start)
    );

    int64_t n_tuples_scanned = 0;
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
    for(auto predicate : query.predicates){
        //  get index from dimension
        auto& current_index = index.at(predicate.column);
        auto& t = cracker_columns.at(predicate.column);
        // if is a range query
        if(predicate.low != predicate.high){
            // search lower_bound of low and high
            auto lower_partition = search_partition(current_index, predicate.low); 
            auto higher_partition = search_partition(current_index, predicate.high); 
            //  if both are in same partition Crack in Three
            if(lower_partition.first == higher_partition.first){
                if(lower_partition.second - lower_partition.first > minimum_partition_size){
                    auto result = t.CrackTableInThree(
                            lower_partition.first, lower_partition.second,
                            predicate.low, predicate.high,
                            1
                            );
                    auto node1 = StandardCrackingNode(predicate.low, result.first);
                    auto node2 = StandardCrackingNode(predicate.high, result.second);
                    if(current_index.find(node1) == current_index.end()){
                        current_index.insert(node1);
                    }
                    if(current_index.find(node2) == current_index.end()){
                        current_index.insert(node2);
                    }
                }
            }
            //  else crack in two each
            else{
                if(lower_partition.second - lower_partition.first > minimum_partition_size){
                    auto node = StandardCrackingNode(predicate.low, 0);
                    if(current_index.find(node) == current_index.end()){
                        node.position = t.CrackTable(
                                lower_partition.first, lower_partition.second,
                                predicate.low, 1
                                );
                        current_index.insert(node);
                    }
                }

                if(higher_partition.second - higher_partition.first > minimum_partition_size){
                    auto node = StandardCrackingNode(predicate.high, 0);
                    if(current_index.find(node) == current_index.end()){
                        node.position = t.CrackTable(
                                higher_partition.first, higher_partition.second,
                                predicate.high, 1
                                );
                        current_index.insert(node);
                    }
                }
            }
        }
        // if is a point query
        else{
            auto lower_partition = search_partition(current_index, predicate.low); 
            if(lower_partition.second - lower_partition.first > minimum_partition_size){
                auto node = StandardCrackingNode(predicate.low, 0);
                if(current_index.find(node) == current_index.end()){
                    node.position = t.CrackTable(
                            lower_partition.first, lower_partition.second,
                            predicate.low, 1
                            );
                    current_index.insert(node);
                }
            }
        }
    }
}

std::vector<char> StandardCracking::search(Query &query){
    // Create a bitvector of chars
    std::vector<char> bitvector(cracker_columns.at(0).row_count());
    // Create bitmap for fist query
    // Select predicate
    auto const& predicate = query.predicates.at(0);
    // Select correct index
    auto const& current_index = index.at(predicate.column);
    // Search the correct partitions to scan 
    int64_t pos_low, pos_high;
    std::tie(pos_low, std::ignore) = search_partition(current_index, predicate.low);
    std::tie(std::ignore, pos_high) = search_partition(current_index, predicate.high);
    // Scan the partitions
    for(int64_t i = pos_low; i <= pos_high; ++i){
        auto value = cracker_columns.at(predicate.column).columns.at(1)->at(i);
        auto id = cracker_columns.at(predicate.column).columns.at(0)->at(i);
        if(predicate.low != predicate.high){
            if(predicate.low <= value && value < predicate.high){
                bitvector.at(id) = '1';
            }
        }else{
            if(predicate.low == value){
                bitvector.at(id) = '1';
            }
        }
    }
    // For each remaining predicate 
    for(int64_t predicate_index = 1; predicate_index < query.predicates.size(); ++predicate_index){
        auto const& predicate = query.predicates.at(predicate_index);
        auto const& current_index = index.at(predicate.column);

        std::tie(pos_low, std::ignore) = search_partition(current_index, predicate.low);
        std::tie(std::ignore, pos_high) = search_partition(current_index, predicate.high);

        for(int64_t i = pos_low; i <= pos_high; ++i){
            auto value = cracker_columns.at(predicate.column).columns.at(1)->at(i);
            auto id = cracker_columns.at(predicate.column).columns.at(0)->at(i);
            if(predicate.low != predicate.high){
                if(predicate.low <= value && value < predicate.high){
                    bitvector.at(id) &= '1';
                }
            }else{
                if(predicate.low == value)
                    bitvector.at(id) &= '1';
            }
        }
    }
    //  search the lower_bounds of low and high
    //  Scan the partition and add to bitvector
    return bitvector;
}

// Returns the partition in which that value is
std::pair<int64_t, int64_t> StandardCracking::search_partition(std::set<StandardCrackingNode> s, float value){
    auto it = s.upper_bound(StandardCrackingNode(value, 0));
    auto upper_node = *it;
    auto low_node = *std::prev(it);
    return std::make_pair(low_node.position, upper_node.position);
}

int64_t StandardCracking::get_node_count(){
    int64_t node_count = 0;
    for(auto &i : index)
        node_count += i.size();
    return node_count;
}

int64_t StandardCracking::biggest_index(){
    int64_t biggest = 0;
    for(auto &i: index){
        if(i.size() > biggest)
            biggest = i.size();
    }
    return biggest;
}

int64_t StandardCracking::smallest_index(){
    int64_t smallest = std::numeric_limits<int64_t>::max();
    for(auto &i : index){
        if(i.size() < smallest)
            smallest = i.size();
    }
    return smallest;
}
