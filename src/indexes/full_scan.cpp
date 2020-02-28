#include "full_scan.hpp"
#include <vector>

FullScan::FullScan(std::map<std::string, std::string> config){}
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
    auto result = Table(1);

    scan_partition(table.get(), query, 0, table->row_count(), &result);

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
    int64_t low, int64_t high,
    Table *table_to_store_results
){
    std::vector<int64_t> qualifying_rows;
    qualifying_rows.reserve(high - low + 1);

    // First we fill the qualifying rows
    auto column = query.predicates[0].column;
    auto low_pred = query.predicates[0].low;
    auto high_pred = query.predicates[0].high;

    size_t qualifying_index = 0;
    for(int64_t row_id = low; row_id < high; row_id++){
        auto value = table->columns[column]->data[row_id];
        if(low_pred <= value && value < high_pred){
            qualifying_rows[qualifying_index] = row_id;
            qualifying_index++;
        }
    }

    // Skip the first predicate
    size_t predicate_index = 1;
    size_t number_of_qualified_rows = qualifying_index;
    for(; predicate_index < query.predicate_count(); ++predicate_index){
        auto column = query.predicates[predicate_index].column;
        auto low_pred = query.predicates[predicate_index].low;
        auto high_pred = query.predicates[predicate_index].high;

        qualifying_index = 0;
        for(auto i = 0; i < number_of_qualified_rows; ++i){
            auto value = table->columns[column]->data[qualifying_rows[i]];
            if(low_pred <= value && value < high_pred){
                qualifying_rows[qualifying_index] = qualifying_rows[i];
                qualifying_index++;
            }
        }
        number_of_qualified_rows = qualifying_index;
    }

    for(auto i = 0; i < number_of_qualified_rows; ++i)
        table_to_store_results->append(
            {static_cast<float>(qualifying_rows[i])}
        );
}
