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
            std::vector<bool>& partition_skip
            );
};

inline std::pair<double, size_t> FullScanCandidateList::scan_partition(
    Table *t,
    Query& query,
    std::vector<std::pair<size_t, size_t>> &partitions,
    std::vector<bool> &partition_skip
){
    assert(partitions.size() == partition_skip.size());
    double sum = 0.0;
    size_t tuples_summed = 0.0;
    for(size_t partition_index = 0; partition_index < partitions.size(); ++partition_index){
        auto low = partitions[partition_index].first;
        auto high = partitions[partition_index].second;

        if(partition_skip[partition_index]){
            for(size_t j = low; j < high; ++j){
                sum += (t->columns[0]->data[j]);
                tuples_summed++;
            }
        }else{
            std::unique_ptr<CandidateList> qualifying_rows = std::make_unique<CandidateList>(0);

            // First we fill the qualifying rows
            auto column = query.predicates[0].column;
            auto low_pred = query.predicates[0].low;
            auto high_pred = query.predicates[0].high;

            for(size_t row_id = low; row_id < high; row_id++){
                auto value = t->columns[column]->data[row_id];
                if(low_pred <= value && value <= high_pred){
                    qualifying_rows->push_back(row_id);
                }
            }

            // Skip the first predicate
            size_t predicate_index = 1;
            size_t number_of_qualified_rows = qualifying_rows->size;
            for(; predicate_index < query.predicate_count(); ++predicate_index){
                column = query.predicates[predicate_index].column;
                low_pred = query.predicates[predicate_index].low;
                high_pred = query.predicates[predicate_index].high;

                auto qualifying_index = 0;
                for(size_t i = 0; i < number_of_qualified_rows; ++i){
                    auto value = t->columns[column]->data[qualifying_rows->get(i)];
                    if(low_pred <= value && value <= high_pred){
                       qualifying_rows->data[qualifying_index] = qualifying_rows->get(i);
                        qualifying_index++;
                    }
                }
                number_of_qualified_rows = qualifying_index;
            }

            for(size_t i = 0; i < number_of_qualified_rows; ++i){
                sum += (t->columns[0]->data[qualifying_rows->get(i)]);
            }
            tuples_summed += number_of_qualified_rows;
        }
    }
    return std::make_pair(sum, tuples_summed);

}
#endif // FULL_SCAN_CANDIDATE_LIST_H
