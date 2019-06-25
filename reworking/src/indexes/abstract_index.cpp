#ifndef ABSTRACT_INDEX
#define ABSTRACT_INDEX

#include "../helpers/helpers.cpp"
#include <vector>

class AbstractIndex
{
protected:
    // Table with copy of the data
    Table *table;
    // Class to keep track of the time/index measurements
    unique_ptr<Measurements> measurements;
public:
    AbstractIndex(){
        measurements = make_unique<Measurements>();
    }
    virtual ~AbstractIndex(){}
    virtual void initialize(Table &table_to_copy) = 0;
    virtual void adapt_index(Query query) = 0;
    virtual unique_ptr<Table> range_query(Query query) = 0;
};
#endif