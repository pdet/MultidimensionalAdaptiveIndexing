#ifndef FULL_SCAN
#define FULL_SCAN

#include "abstract_index.cpp"

class FullScan : public AbstractIndex
{
public:
    FullScan(){}
    ~FullScan(){}

    void initialize(const shared_ptr<Table> table_to_copy){
        auto start = measurements->time();

        // Simply copies the pointer of the table, since it does not change anything
        table = table_to_copy;

        measurements->initialization_time = measurements->time() - start;;
    }

    void adapt_index(const shared_ptr<Query> query){
        // Zero adaptation for full scan
        measurements->adaptation_time.push_back(
            Measurements::difference(measurements->time(), measurements->time())
        );
    }

    unique_ptr<Table> range_query(const shared_ptr<Query> query){
        auto start = measurements->time();

        // Scan the table and returns a materialized view of the result.
        auto result = make_unique<Table>(table->col_count());
        scan_partition(table, query, 0, table->row_count(), move(result));

        auto end = measurements->time();

        measurements->query_time.push_back(
            Measurements::difference(end, start)
        );
        return result;
    }

    static void scan_partition(
        shared_ptr<Table> table, shared_ptr<Query> query,
        size_t low, size_t high,
        unique_ptr<Table> table_to_store_results
    ){
        for(size_t row_id = low; row_id < high; row_id++)
            if(condition_is_true(table, query, row_id))
                table_to_store_results->append(table->materialize_row(row_id));
    }
private:
    bool static condition_is_true(shared_ptr<Table> table, shared_ptr<Query> query, size_t row_index){
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