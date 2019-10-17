#ifndef FULL_SCAN_H
#define FULL_SCAN_H

#include "abstract_index.hpp"
#include <string>
#include <map>

class FullScan : public AbstractIndex
{
public:
    FullScan(std::map<std::string, std::string> config);
    ~FullScan();

    string name() override{
        return "Full Scan";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    static void scan_partition(
        Table *table, Query& query,
        size_t low, size_t high,
        Table *table_to_store_results
    );
};
#endif // FULL_SCAN_H
