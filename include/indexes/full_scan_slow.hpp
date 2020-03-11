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

    string name() override{
        return "FullScanSlow(baseline)";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    static void scan_partition(
        IdxTbl *t, Query& query,
        int64_t low, int64_t high,
        Table *table_to_store_results
    );
};
#endif // FULL_SCAN_SLOW_H
