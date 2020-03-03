#include "full_scan_slow.hpp"
#include <vector>

FullScanSlow::FullScanSlow(std::map<std::string, std::string> config){}
FullScanSlow::~FullScanSlow(){}

void FullScanSlow::initialize(Table *table_to_copy){
    auto start = measurements->time();

    // Simply copies the pointer of the table, since it does not change anything
    table = make_unique<Table>(table_to_copy);

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
}

void FullScanSlow::adapt_index(Query& query){
    // Zero adaptation for full scan
    measurements->append("adaptation_time",
        std::to_string(
            Measurements::difference(measurements->time(), measurements->time())
        )
    );
}

Table FullScanSlow::range_query(Query& query){
    auto start = measurements->time();


    // Scan the table and returns the row_ids
    auto result = Table(1);

    scan_partition(table.get(), query, 0, table->row_count(), &result);

    auto end = measurements->time();

    // ******************
    measurements->append(
        "query_time",
        std::to_string(Measurements::difference(end, start))
    );

    measurements->append("tuples_scanned", std::to_string(table->row_count()));

    return result;
}

void FullScanSlow::scan_partition(
    Table *t, Query& query,
    int64_t low, int64_t high,
    Table *table_to_store_results
){
    std::vector<int64_t> qualifying_rows;
    qualifying_rows.reserve(high - low + 1);

    bool first = true;

    for(auto predicate : query.predicates){
        auto column = predicate.column;
        auto low_pred = predicate.low;
        auto high_pred = predicate.high;
        
        // First time we have to fill the qualyfing rows
        if(first){

            for(int64_t row_id = low; row_id < high; row_id++){
                auto value = t->columns.at(column)->at(row_id);
                if(low_pred <= value && value < high_pred)
                    qualifying_rows.push_back(row_id);
            }
            first = false;
        }
        else{
            std::vector<int64_t> temp_qualifying_rows;
            temp_qualifying_rows.reserve(high - low + 1);
            for(auto row_id : qualifying_rows){
                auto value = t->columns.at(column)->at(row_id);
                if(low_pred <= value && value < high_pred)
                    temp_qualifying_rows.push_back(row_id);
            }
            qualifying_rows = temp_qualifying_rows;
        }
    }

    for(auto qualifying_row : qualifying_rows)
        table_to_store_results->append({static_cast<float>(qualifying_row)});
}
