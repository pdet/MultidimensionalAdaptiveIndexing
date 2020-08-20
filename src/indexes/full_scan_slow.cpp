#include "full_scan_slow.hpp"
#include <vector>

using namespace std;

FullScanSlow::FullScanSlow(std::map<std::string, std::string> /*config*/){}
FullScanSlow::~FullScanSlow(){}

void FullScanSlow::initialize(Table *table_to_copy){
    auto start = measurements->time();

    // Simply copies the pointer of the table, since it does not change anything
    table = std::make_unique<Table>(table_to_copy);

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
}

void FullScanSlow::adapt_index(Query &query) {
    // Zero adaptation for full scan
    measurements->append("adaptation_time",
        std::to_string(
            Measurements::difference(measurements->time(), measurements->time())
        )
    );
}

unique_ptr<Table> FullScanSlow::range_query(Query &query) {
    auto start = measurements->time();


    // Scan the table and returns the row_ids
    std::vector<std::pair<size_t, size_t> > partitions;
    partitions.push_back(std::make_pair(0, table->row_count()));
    std::vector<std::vector<bool>> per_partition_attribute_skip(partitions.size(), std::vector<bool>(table->col_count(), false));
    auto result = FullScanSlow::scan_partition(table.get(), query, partitions, per_partition_attribute_skip);

    auto end = measurements->time();

    // ******************
    measurements->append(
        "query_time",
        std::to_string(Measurements::difference(end, start))
    );

    measurements->append("tuples_scanned", std::to_string(table->row_count()));

    measurements->append(
        "scan_overhead",
        std::to_string(
            table->row_count()/static_cast<float>(result.second)
        )
    );
    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(result.first), static_cast<float>(result.second)};
    t->append(row);
    return t;
}

pair<double, size_t> FullScanSlow::scan_partition(
        Table *t,
        Query& query,
        std::vector<std::pair<size_t, size_t> >& partitions,
        std::vector<std::vector<bool>>& /*per_partition_attribute_skip*/
){
    double sum = 0.0;
    size_t tuples_summed = 0.0;
    for(size_t partition_index = 0; partition_index < partitions.size(); ++partition_index){
        auto low = partitions[partition_index].first;
        auto high = partitions[partition_index].second;
        std::vector<int64_t> qualifying_rows;
        qualifying_rows.reserve(high - low + 1);
        bool first = true;

        for(auto predicate : query.predicates){
            auto column = predicate.column;
            auto low_pred = predicate.low;
            auto high_pred = predicate.high;

            // First time we have to fill the qualyfing rows
            if(first){

                for(size_t row_id = low; row_id < high; row_id++){
                    auto value = t->columns[column]->data[row_id];
                    if(low_pred <= value && value <= high_pred)
                        qualifying_rows.push_back(row_id);
                }
                first = false;
            }
            else{
                std::vector<int64_t> temp_qualifying_rows;
                temp_qualifying_rows.reserve(high - low + 1);
                for(auto row_id : qualifying_rows){
                    auto value = t->columns[column]->data[row_id];
                    if(low_pred <= value && value <= high_pred)
                        temp_qualifying_rows.push_back(row_id);
                }
                qualifying_rows = temp_qualifying_rows;
            }
        }

        for(auto qualifying_row : qualifying_rows){
            sum += static_cast<double>(t->columns[0]->data[qualifying_row]);
            tuples_summed++;
        }
    }
    return make_pair(sum, tuples_summed);
}
