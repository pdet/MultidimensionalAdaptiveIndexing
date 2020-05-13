#ifndef FULL_SCAN_H
#define FULL_SCAN_H

#include "abstract_index.hpp"
#include <string>
#include <map>
#include "bitvector.hpp"

class FullScan : public AbstractIndex
{
public:
    const static size_t ID = 1;
    FullScan(std::map<std::string, std::string> config);
    ~FullScan();

    std::string name() override{
        return "FullScan (BitVector)";
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

inline std::pair<double, size_t> FullScan::scan_partition(
    Table *t,
    Query& query,
    std::vector<std::pair<size_t, size_t> >& partitions,
    std::vector<bool>& partition_skip
){
    assert(partitions.size() == partition_skip.size());
    double sum = 0.0;
    size_t tuples_summed = 0;
    for(size_t partition_index = 0; partition_index < partitions.size(); ++partition_index){
        auto low = partitions[partition_index].first;
        auto high = partitions[partition_index].second;

        if(partition_skip[partition_index]){
            for(size_t j = low; j < high; ++j){
                sum += (t->columns[0]->data[j]);
                tuples_summed++;
            }
        }else{
            size_t partition_size = high-low;
            BitVector bit_vector(partition_size);

            {
                // initialize bit_vector on the first column
                auto low_pred = query.predicates[0].low;
                auto high_pred = query.predicates[0].high;

                for(size_t i = 0; i < partition_size; i++){
                    auto value = t->columns[0]->data[i + low];
                    bit_vector.set(i, low_pred <= value && value <= high_pred);
                }
            }

            for(size_t col = 1; col < t->col_count(); ++col){
                auto low_pred = query.predicates[col].low;
                auto high_pred = query.predicates[col].high;

                for(size_t i = 0; i < partition_size; i++){
                    if(bit_vector.get(i)){
                        auto value = t->columns[col]->data[i + low];
                        bit_vector.set(i, low_pred <= value && value <= high_pred);
                    }
                }
            }

            for(size_t i = 0; i < bit_vector.size(); ++i){
                if(bit_vector.get(i)){
                    sum += (t->columns[0]->data[i + low]);
                    tuples_summed++;
                }
            }
        }
    }
    return std::make_pair(sum, tuples_summed);

}
#endif // FULL_SCAN_H
