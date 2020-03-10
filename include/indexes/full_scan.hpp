#ifndef FULL_SCAN_H
#define FULL_SCAN_H

#include "abstract_index.hpp"
#include <string>
#include <map>

class FullScan : public AbstractIndex
{
public:
    const static size_t ID = 1;
    FullScan(std::map<std::string, std::string> config);
    ~FullScan();

    string name() override{
        return "FullScan";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    static void scan_partition(
        Table *t, Query& query,
        int64_t low, int64_t high,
        Table *table_to_store_results
    );
};
#endif // FULL_SCAN_H
