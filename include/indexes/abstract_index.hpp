#ifndef ABSTRACT_INDEX_H
#define ABSTRACT_INDEX_H

#include "../helpers/table.hpp"
#include "../helpers/measurements.hpp"
#include "../helpers/query.hpp"
#include <vector>

class AbstractIndex
{
protected:
    // Table with copy of the data
    shared_ptr<Table> table;
public:
    // Class to keep track of the time/index measurements
    unique_ptr<Measurements> measurements;

    AbstractIndex(){
        measurements = make_unique<Measurements>();
    }
    virtual ~AbstractIndex(){}
    virtual void initialize(const shared_ptr<Table> table_to_copy) = 0;
    virtual void adapt_index(Query& query) = 0;
    virtual shared_ptr<Table> range_query(Query& query) = 0;
    virtual string name() = 0;
};
#endif // ABSTRACT_INDEX_H