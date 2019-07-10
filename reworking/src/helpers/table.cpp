#ifndef TABLE
#define TABLE

#include "column.cpp"
#include <vector>
#include <cstdint>
#include <iostream>

using namespace std;

class Table{
private:
    size_t number_of_rows;
    size_t number_of_columns;
public:
    vector<unique_ptr<Column>> columns;

    Table(vector<vector<float> > &columns_to_be_copied){
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
        for(size_t i = 0; i < number_of_columns; ++i){
            columns.at(i) = make_unique<Column>();
        }
        number_of_rows = 0;
    }

    ~Table(){}

    vector<float> materialize_row(size_t row_index){
        vector<float> row(col_count());
        for(size_t col = 0; col < col_count(); col++){
            row.at(col) = columns.at(col)->at(row_index);
        }
        return row;
    }

    void append(vector<float> row){
        assert(row.size() == number_of_columns);
        for(size_t col = 0; col < row.size(); col++){
            columns.at(col)->append(row.at(col));
        }
        number_of_rows++;
    }

    void exchange(size_t index1, size_t index2){
        for(size_t column_index = 0; column_index < number_of_columns; ++column_index){
            auto value1 = columns.at(column_index)->at(index1);
            auto value2 = columns.at(column_index)->at(index2);

            auto tmp = value1;
            columns.at(column_index)->assign(index1, value2);
            columns.at(column_index)->assign(index2, tmp);
        }
    }

    // Cracks table from position i = low until i == high
    // on column (c) with key (element)
    // Returns in which position the key would end
    int64_t CrackTable(int64_t low, int64_t high, int64_t element, int64_t c)
    {
        int64_t x1 = low;
        int64_t x2 = high;

        while (x1 <= x2)
        {
            if (columns.at(c)->at(x1) < element)
                x1++;
            else
            {
                while (x2 >= x1 && (columns.at(c)->at(x2) >= element))
                    x2--;
                if (x1 < x2)
                {
                    exchange(x1, x2);
                    x1++;
                    x2--;
                }
            }
        }
        if (x1 < x2){
            cout << "Not all elements were inspected!" << endl;
            exit(-1);
        }
        x1--;
        return x1;
    }

    size_t row_count(){
        return number_of_rows;
    }

    size_t col_count(){
        return number_of_columns;
    }
};

#endif