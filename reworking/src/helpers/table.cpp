#ifndef TABLE
#define TABLE

#include "column.cpp"
#include <vector>
#include <cstdint>

using namespace std;

class Table{
private:
    size_t number_of_rows;
    size_t number_of_columns;
public:
    vector<unique_ptr<Column>> columns;

    Table(vector<vector<int64_t> > &columns_to_be_copied){
        number_of_columns = columns_to_be_copied.size();
        number_of_rows = columns_to_be_copied.at(0).size();

        columns.resize(number_of_columns);
        for (size_t col = 0; col < number_of_columns; col++){
            columns.at(col) = make_unique<Column>(
                columns_to_be_copied.at(col)
            );
        }
    }

    Table(size_t number_of_columns) : number_of_columns(number_of_columns){
        columns.resize(number_of_columns);
        number_of_rows = 0;
    }

    ~Table(){}

    vector<int64_t> materialize_row(size_t row_index){
        vector<int64_t> row(col_count());
        for(size_t col = 0; col < col_count(); col++){
            row.at(col) = columns.at(col)->at(row_index);
        }
        return row;
    }

    void append(vector<int64_t> row){
        assert(row.size() == number_of_columns);
        for(size_t col = 0; col < row.size(); col++){
            columns.at(col)->append(row.at(col));
        }
        number_of_rows++;
    }

    size_t row_count(){
        return number_of_rows;
    }

    size_t col_count(){
        return number_of_columns;
    }
};

#endif