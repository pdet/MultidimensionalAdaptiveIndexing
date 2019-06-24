#ifndef FULL_SCAN
#define FULL_SCAN

#include "abstract_index.cpp"

class FullScan : AbstractIndex
{
public:
    FullScan();
    ~FullScan();

    void initialize(Table &table_to_copy){
        table = table_to_copy;
    }
    void adapt_index(Query query){}
    unique_ptr<Table> range_query(Query query){
        auto result = make_unique<Table>(table.col_count());
        for(size_t row_id = 0; row_id < table.row_count(); row_id++){
            if(condition_is_true(query, row_id))
                result->append(table.materialize_row(row_id));
        }
        return result;
    }
private:
    bool condition_is_true(Query query, size_t row_index){
        for(size_t predicate_index = 0; predicate_index < query.predicate_count(); predicate_index++){
            auto column = query.predicates.at(predicate_index)->column;
            auto low = query.predicates.at(predicate_index)->low;
            auto high = query.predicates.at(predicate_index)->high;

            auto value = table.columns.at(column)->at(row_index);
            if(!(low <= value && value < high))
                return false;
        }
        return true;
    }
};
#endif