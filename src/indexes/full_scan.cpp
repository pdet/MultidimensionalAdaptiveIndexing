#include "full_scan.hpp"
#include <vector>
#include <cassert>
#include <iostream>
#include "bitvector.hpp"

using namespace std;

FullScan::FullScan(std::map<std::string, std::string> /*config*/){}
FullScan::~FullScan(){}

void FullScan::initialize(Table *table_to_copy){
    auto start = measurements->time();

    // Simply copies the pointer of the table, since it does not change anything
    table = make_unique<Table>(table_to_copy);

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
}

void FullScan::adapt_index(Query& /*query*/){
void FullScan::adapt_index(Table *originalTable,Query& query){
    // Zero adaptation for full scan
    measurements->append(
        "adaptation_time",
        std::to_string(
            Measurements::difference(measurements->time(), measurements->time())
        )
    );
}

std::unique_ptr<Table>  FullScan::range_query(Table *originalTable,Query& query){
    auto start = measurements->time();


    // Scan the table and returns a materialized view of the result.
    std::vector<std::pair<size_t, size_t> > partitions;
    partitions.push_back(std::make_pair(0, table->row_count()));
    std::vector<bool> partition_skip (partitions.size(), false);
    auto result = FullScan::scan_partition(table.get(), query, partitions, partition_skip);

    auto end = measurements->time();

    measurements->append(
        "scan_time",
        std::to_string(Measurements::difference(end, start))
    );

    // Before returning the result, update the statistics.
    measurements->append("tuples_scanned", std::to_string(table->row_count()));

    measurements->append(
        "scan_overhead",
        std::to_string(
            table->row_count()/static_cast<float>(result->row_count())
        )
    );

    return result;
}

unique_ptr<Table> FullScan::scan_partition(
    Table *t,
    Query& query,
    std::vector<std::pair<size_t, size_t> >& partitions,
    std::vector<bool>& partition_skip
){
    assert(partitions.size() == partition_skip.size());
    auto table_to_store_results = make_unique<Table>(1); 
    for(size_t partition_index = 0; partition_index < partitions.size(); ++partition_index){
        auto low = partitions[partition_index].first;
        auto high = partitions[partition_index].second;

        if(partition_skip[partition_index]){
            for(size_t j = low; j < high; ++j){
                table_to_store_results->append(
                        &(t->columns[0]->data[j])
                        );
            }
        }else{
            BitVector bit_vector(high-low, 1);

            for(size_t col = 0; col < t->col_count(); ++col){
                BitVector temp_bit_vector(high-low, 0);
                auto low_pred = query.predicates[col].low;
                auto high_pred = query.predicates[col].high;

                for(size_t i = 0; i < bit_vector.size(); i++){
                    auto value = t->columns[col]->data[i + low];
                    if(low_pred <= value && value <= high_pred){
                        temp_bit_vector.set(i);
                    }
                }

                bit_vector.bitwise_and(temp_bit_vector);
            }

            for(size_t i = 0; i < bit_vector.size(); ++i){
                if(bit_vector.get(i) == true){
                    table_to_store_results->append(
                            &(t->columns[0]->data[low + i])
                            );
                }
            }
        }
    }
    return table_to_store_results;

}
