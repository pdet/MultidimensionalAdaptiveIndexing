#include "full_scan.hpp"
#include <vector>

FullScan::FullScan(){}
FullScan::~FullScan(){}

void FullScan::initialize(Table *table_to_copy){
    auto start = measurements->time();

    // Simply copies the pointer of the table, since it does not change anything
    table = make_unique<Table>(table_to_copy);

    auto end = measurements->time();

    measurements->initialization_time = Measurements::difference(end, start);
}

void FullScan::adapt_index(Query& query){
    // Zero adaptation for full scan
    measurements->adaptation_time.push_back(
        Measurements::difference(measurements->time(), measurements->time())
    );
}

Table FullScan::range_query(Query& query){
    auto start = measurements->time();


    // Scan the table and returns a materialized view of the result.
    auto result = Table(table->col_count());

    scan_partition(table.get(), query, 0, table->row_count() - 1, &result);

    auto end = measurements->time();

    measurements->query_time.push_back(
        Measurements::difference(end, start)
    );

    // Before returning the result, update the statistics.
    measurements->number_of_nodes.push_back(0);
    measurements->max_height.push_back(0);
    measurements->min_height.push_back(0);
    measurements->memory_footprint.push_back(0);
    measurements->tuples_scanned.push_back(table->row_count());

    return result;
}

void FullScan::scan_partition(
    Table *table, Query& query,
    size_t low, size_t high,
    Table *table_to_store_results
){
    std::vector<size_t> qualifying_rows;
    qualifying_rows.reserve(high - low + 1);

    bool first = true;

    for(auto predicate : query.predicates){
        auto column = predicate.column;
        auto low_pred = predicate.low;
        auto high_pred = predicate.high;
        // First time we have to fill the qualyfing rows
        if(first){

            // If it is a range query
            if(low_pred != high_pred){
                for(size_t row_id = low; row_id <= high; row_id++){
                    auto value = table->columns.at(column)->at(row_id);
                    if(low_pred <= value && value < high_pred)
                        qualifying_rows.push_back(row_id);
                }
            }
            // If it is a point query
            else{
                for(size_t row_id = low; row_id <= high; row_id++){
                    auto value = table->columns.at(column)->at(row_id);
                    if(low_pred == value)
                        qualifying_rows.push_back(row_id);
                }
            }
            first = false;
        }
        else{
            std::vector<size_t> temp_qualifying_rows;
            temp_qualifying_rows.reserve(high - low + 1);
            // If it is a range query
            if(low_pred != high_pred){
                for(auto row_id : qualifying_rows){
                    auto value = table->columns.at(column)->at(row_id);
                    if(low_pred <= value && value < high_pred)
                        temp_qualifying_rows.push_back(row_id);
                }
            }
            // If it is a point query
            else{
                for(auto row_id : qualifying_rows){
                    auto value = table->columns.at(column)->at(row_id);
                    if(low_pred == value)
                        temp_qualifying_rows.push_back(row_id);
                }
            }
            qualifying_rows = temp_qualifying_rows;
        }
    }

    for(auto qualifying_row : qualifying_rows)
        table_to_store_results->append(table->materialize_row(qualifying_row));
}
