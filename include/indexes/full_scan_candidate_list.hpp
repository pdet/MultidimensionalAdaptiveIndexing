#ifndef FULL_SCAN_CANDIDATE_LIST_H
#define FULL_SCAN_CANDIDATE_LIST_H

#include "abstract_index.hpp"
#include <string>
#include <map>
#include "candidate_list.hpp"

class FullScanCandidateList : public AbstractIndex
{
public:
    const static size_t ID = 111;
    FullScanCandidateList(std::map<std::string, std::string> config);
    ~FullScanCandidateList();

    std::string name() override{
        return "FullScan (Candidate List)";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query &query) override;

    std::unique_ptr<Table> range_query(Query &query) override;

    static std::pair<double, size_t> scan_partition(
            Table *t,
            Query& query,
            std::vector<std::pair<size_t, size_t> >& partitions,
            std::vector<std::vector<bool>> &per_partition_attribute_skip
            );
};

inline std::pair<double, size_t> FullScanCandidateList::scan_partition(
    Table *t,
    Query& query,
    std::vector<std::pair<size_t, size_t>> &partitions,
    std::vector<std::vector<bool>> &per_partition_attribute_skip
){
    assert(partitions.size() == per_partition_attribute_skip.size());
    double sum = 0.0;
    size_t tuples_summed = 0.0;
    for(size_t partition_index = 0; partition_index < partitions.size(); ++partition_index){
        auto low = partitions[partition_index].first;
        auto high = partitions[partition_index].second;

        std::unique_ptr<CandidateList> qualifying_rows = std::make_unique<CandidateList>(0);

        // Handle the first predicate
        // If we should skip the first attribute
        auto column = query.predicates[0].column;
        if(per_partition_attribute_skip[partition_index][column]){
            for(size_t row_id = low; row_id < high; row_id++){
                qualifying_rows->push_back(row_id);
            }
        }else{
            // First we fill the qualifying rows
            auto low_pred = query.predicates[0].low;
            auto high_pred = query.predicates[0].high;

            for(size_t row_id = low; row_id < high; row_id++){
                auto value = t->columns[column]->data[row_id];
                qualifying_rows->maybe_push_back(row_id, low_pred <= value && value <= high_pred);
            }
        }


        // Skip the first predicate as we already processed it
        for(size_t predicate_index = 1; predicate_index < query.predicate_count(); ++predicate_index){

            if(qualifying_rows->size == 0)
                break;
            column = query.predicates[predicate_index].column;

            if(per_partition_attribute_skip[partition_index][column]){
                /* Do Nothing */
            }else{
                auto low_pred = query.predicates[predicate_index].low;
                auto high_pred = query.predicates[predicate_index].high;

                auto qualifying_index = 0;
                for(size_t i = 0; i < qualifying_rows->size; ++i){
                    auto value = t->columns[column]->data[qualifying_rows->get(i)];
                    int matching = low_pred <= value && value <= high_pred;
                    qualifying_rows->data[qualifying_index] = qualifying_rows->get(i);
                    qualifying_index+= matching;
                }
                qualifying_rows->size = qualifying_index;
            }

        }

        for(size_t i = 0; i < qualifying_rows->size; ++i){
            sum += (t->columns[0]->data[qualifying_rows->get(i)]);
        }
        tuples_summed += qualifying_rows->size;
    }
    return std::make_pair(sum, tuples_summed);

}
#endif // FULL_SCAN_CANDIDATE_LIST_H
