#ifndef ABSTRACT_INDEX
#define ABSTRACT_INDEX

#include "../helpers/helpers.cpp"
#include <vector>

class AbstractIndex
{
protected:
    // Table with copy of the data
    shared_ptr<Table> table;
    // Class to keep track of the time/index measurements
    unique_ptr<Measurements> measurements;
public:
    AbstractIndex(){
        measurements = make_unique<Measurements>();
    }
    virtual ~AbstractIndex(){}
    virtual void initialize(const shared_ptr<Table> table_to_copy) = 0;
    virtual void adapt_index(const shared_ptr<Query> query) = 0;
    virtual unique_ptr<Table> range_query(const shared_ptr<Query> query) = 0;
};
#endif