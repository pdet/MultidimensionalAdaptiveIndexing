#ifndef CRACKING_KDTREE_BROAD
#define CRACKING_KDTREE_BROAD

#include "abstract_index.cpp"

class CrackingKDTreeBroad : public AbstractIndex
{
public:
    CrackingKDTreeBroad(){}
    ~CrackingKDTreeBroad(){}

    void initialize(const shared_ptr<Table> table_to_copy){
        table = table_to_copy;
    }

    void adapt_index(const shared_ptr<Query> query){}

    unique_ptr<Table> range_query(const shared_ptr<Query> query){
        auto result = make_unique<Table>(table->col_count());
        for(size_t row_id = 0; row_id < table->row_count(); row_id++){
            if(condition_is_true(query, row_id))
                result->append(table->materialize_row(row_id));
        }
        return result;
    }
private:
    bool condition_is_true(shared_ptr<Query> query, size_t row_index){
        for(size_t predicate_index = 0; predicate_index < query->predicate_count(); predicate_index++){
            auto column = query->predicates.at(predicate_index)->column;
            auto low = query->predicates.at(predicate_index)->low;
            auto high = query->predicates.at(predicate_index)->high;

            auto value = table->columns.at(column)->at(row_index);
            if(!(low <= value && value < high))
                return false;
        }
        return true;
    }
};
#endif