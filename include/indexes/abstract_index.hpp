#ifndef ABSTRACT_INDEX_H
#define ABSTRACT_INDEX_H

#include "../helpers/table.hpp"
#include "../helpers/measurements.hpp"
#include "../helpers/query.hpp"
#include <vector>
#include <string>

class AbstractIndex
{
protected:
    // Table with copy of the data
    unique_ptr<Table> table;
public:
    // Class to keep track of the time/index measurements
    unique_ptr<Measurements> measurements;

    AbstractIndex(){
        measurements = make_unique<Measurements>();
    }
    virtual ~AbstractIndex(){}
    virtual void initialize(Table *table_to_copy) = 0;
    virtual void adapt_index(Query& query) = 0;
    virtual Table range_query(Query& query) = 0;
    virtual string name() = 0;
    virtual void draw_index(std::string path){}
};
#endif // ABSTRACT_INDEX_H
