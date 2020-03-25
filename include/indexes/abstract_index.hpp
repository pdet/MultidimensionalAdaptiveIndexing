#ifndef ABSTRACT_INDEX_H
#define ABSTRACT_INDEX_H

#include "measurements.hpp"
#include "query.hpp"
#include "table.hpp"
#include <index_table.hpp>
#include <string>

class AbstractIndex
{
protected:
    // Table with copy of the data
    std::unique_ptr<Table> table;
    int64_t n_tuples_scanned_before_adapting;
public:
    // Class to keep track of the time/index measurements
    std::unique_ptr<Measurements> measurements;

    AbstractIndex(){
        measurements = std::make_unique<Measurements>();
    }
    virtual ~AbstractIndex(){}
    virtual void initialize(Table *table_to_copy) = 0;
    virtual void adapt_index(Table *originalTable,Query& query) = 0;
    virtual std::unique_ptr<Table> range_query(Table *originalTable,Query& query) = 0;
    virtual std::string name() = 0;
    virtual void draw_index(std::string path){}
};
#endif // ABSTRACT_INDEX_H
