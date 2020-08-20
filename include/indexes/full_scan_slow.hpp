#ifndef FULL_SCAN_SLOW_H
#define FULL_SCAN_SLOW_H

#include "abstract_index.hpp"
#include <string>
#include <map>

class FullScanSlow : public AbstractIndex
{
public:
    const static size_t ID = 0;
    FullScanSlow(std::map<std::string, std::string> config);
    ~FullScanSlow();

    std::string name() override{
        return "FullScanSlow(baseline)";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query &query) override;

  std::unique_ptr<Table> range_query(Query &query) override;

    static std::pair<double, size_t> scan_partition(
            Table *t,
            Query& query,
            std::vector<std::pair<size_t, size_t> >& partitions,
            std::vector<std::vector<bool>>& partition_skip
            );

};
#endif // FULL_SCAN_SLOW_H
