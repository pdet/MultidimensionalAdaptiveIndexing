#include "full_scan_candidate_list.hpp"
#include <vector>
#include <cassert>

using namespace std;

FullScanCandidateList::FullScanCandidateList(std::map<std::string, std::string> /*config*/){}
FullScanCandidateList::~FullScanCandidateList(){}

void FullScanCandidateList::initialize(Table *table_to_copy){
    auto start = measurements->time();

    // Simply copies the pointer of the table, since it does not change anything
    table = make_unique<Table>(table_to_copy);

    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
}

void FullScanCandidateList::adapt_index(Query& /*query*/){
    // Zero adaptation for full scan
    measurements->append(
        "adaptation_time",
        std::to_string(
            Measurements::difference(measurements->time(), measurements->time())
        )
    );
}

std::unique_ptr<Table> FullScanCandidateList::range_query(Query& query){
    auto start = measurements->time();


    // Scan the table and returns a materialized view of the result.
    std::vector<std::pair<size_t, size_t> > partitions;
    partitions.push_back(std::make_pair(0, table->row_count()));
    std::vector<bool> partition_skip (partitions.size(), false);
    auto result = FullScanCandidateList::scan_partition(table.get(), query, partitions, partition_skip);

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

unique_ptr<Table> FullScanCandidateList::scan_partition(
    Table *t,
    Query& query,
    std::vector<std::pair<size_t, size_t>> &partitions,
    std::vector<bool> &partition_skip
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
            std::unique_ptr<int64_t[]> qualifying_rows{new int64_t[high - low + 1]};

            // First we fill the qualifying rows
            auto column = query.predicates[0].column;
            auto low_pred = query.predicates[0].low;
            auto high_pred = query.predicates[0].high;

            size_t qualifying_index = 0;
            for(size_t row_id = low; row_id < high; row_id++){
                auto value = t->columns[column]->data[row_id];
                if(low_pred <= value && value <= high_pred){
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
                for(size_t i = 0; i < number_of_qualified_rows; ++i){
                    auto value = t->columns[column]->data[qualifying_rows[i]];
                    if(low_pred <= value && value <= high_pred){
                        qualifying_rows[qualifying_index] = qualifying_rows[i];
                        qualifying_index++;
                    }
                }
                number_of_qualified_rows = qualifying_index;
            }

            for(size_t i = 0; i < number_of_qualified_rows; ++i){
                table_to_store_results->append(
                        &(t->columns[0]->data[qualifying_rows[i]])
                        );
            }

        }
    }
    return table_to_store_results;

}
