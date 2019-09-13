#ifndef FULL_SCAN_H
#define FULL_SCAN_H

#include "abstract_index.hpp"

class FullScan : public AbstractIndex
{
public:
    FullScan();
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

private:
    bool static condition_is_true(Table *table, Query& query, size_t row_index);
};
#endif // FULL_SCAN_H
