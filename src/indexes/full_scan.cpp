#include "full_scan.hpp"

FullScan::FullScan(){}
FullScan::~FullScan(){}

void FullScan::initialize(const shared_ptr<Table> table_to_copy){
    auto start = measurements->time();

    // Simply copies the pointer of the table, since it does not change anything
    table = table_to_copy;

    measurements->initialization_time = measurements->time() - start;;
}

void FullScan::adapt_index(Query& query){
    // Zero adaptation for full scan
    measurements->adaptation_time.push_back(
        Measurements::difference(measurements->time(), measurements->time())
    );
}

shared_ptr<Table> FullScan::range_query(Query& query){
    auto start = measurements->time();


    // Scan the table and returns a materialized view of the result.
    auto result = make_shared<Table>(table->col_count());

    scan_partition(table, query, 0, table->row_count() - 1, result);

    auto end = measurements->time();

    measurements->query_time.push_back(
        Measurements::difference(end, start)
    );

    return result;
}

void FullScan::scan_partition(
    shared_ptr<Table> table, Query& query,
    size_t low, size_t high,
    shared_ptr<Table> table_to_store_results
){
    for(size_t row_id = low; row_id <= high; row_id++)
        if(condition_is_true(table, query, row_id))
            table_to_store_results->append(table->materialize_row(row_id));
}

bool FullScan::condition_is_true(shared_ptr<Table> table, Query& query, size_t row_index){
    for(auto predicate : query.predicates){
        auto column = predicate.column;
        auto low = predicate.low;
        auto high = predicate.high;

        auto value = table->columns.at(column)->at(row_index);
        if(!(low <= value && value < high))
            return false;
    }
    return true;
}