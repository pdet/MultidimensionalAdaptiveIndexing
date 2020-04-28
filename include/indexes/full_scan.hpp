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

    std::string name() override{
        return "FullScan";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    std::unique_ptr<Table> range_query(Query& query) override;

    static std::unique_ptr<Table> scan_partition(
            Table *t,
            Query& query,
            std::vector<std::pair<size_t, size_t> >& partitions,
            std::vector<bool>& partition_skip
            );
};
#endif // FULL_SCAN_H
