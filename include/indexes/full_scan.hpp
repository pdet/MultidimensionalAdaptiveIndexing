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

    void initialize(const shared_ptr<Table> table_to_copy) override;

    void adapt_index(Query& query) override;

    shared_ptr<Table> range_query(Query& query) override;

    static void scan_partition(
        shared_ptr<Table> table, Query& query,
        size_t low, size_t high,
        shared_ptr<Table> table_to_store_results
    );

private:
    bool static condition_is_true(shared_ptr<Table> table, Query& query, size_t row_index);
};
#endif // FULL_SCAN_H
