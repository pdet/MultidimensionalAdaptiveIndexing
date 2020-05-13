#include "full_scan.hpp"
#include <vector>
#include <cassert>

using namespace std;

FullScan::FullScan(std::map<std::string, std::string> /*config*/){}
FullScan::~FullScan(){}

void FullScan::initialize(Table *table_to_copy){

    // Simply copies the pointer of the table, since it does not change anything
    table = make_unique<Table>(table_to_copy);

    // FIXME: should just copy the pointer, instead of copying the entire table.
    //          for now it can be left like this and make init time equal 0
    auto start = measurements->time();
    auto end = measurements->time();

    measurements->append(
        "initialization_time",
        std::to_string(Measurements::difference(end, start))
    );
}

void FullScan::adapt_index(Query &query) {
    // Zero adaptation for full scan
    measurements->append(
        "adaptation_time",
        std::to_string(
            Measurements::difference(measurements->time(), measurements->time())
        )
    );
}

unique_ptr<Table> FullScan::range_query(Query &query) {
    auto start = measurements->time();


    // Scan the table and returns a materialized view of the result.
    std::vector<std::pair<size_t, size_t> > partitions;
    partitions.push_back(std::make_pair(0, table->row_count()));
    std::vector<bool> partition_skip (partitions.size(), false);
    auto result = FullScan::scan_partition(table.get(), query, partitions, partition_skip);

    auto end = measurements->time();

    measurements->append(
        "scan_time",
        std::to_string(Measurements::difference(end, start))
    );

    // Before returning the result, update the statistics.
    measurements->append("tuples_scanned", std::to_string(table->row_count()));

    measurements->append(
        "scan_overhead",
        std::to_string(
            table->row_count()/static_cast<float>(result.second)
        )
    );

    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(result.first), static_cast<float>(result.second)};
    t->append(row);
    return t;
}
