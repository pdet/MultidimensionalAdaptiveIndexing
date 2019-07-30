#include "column.hpp"
#include "table.hpp"
#include <vector>
#include <cstdint>
#include <iostream>

using namespace std;
Table::Table(vector<vector<float> > &columns_to_be_copied){
    number_of_columns = columns_to_be_copied.size();
    number_of_rows = columns_to_be_copied.at(0).size();

    columns.resize(number_of_columns);
    for (size_t col = 0; col < number_of_columns; col++){
        columns.at(col) = make_unique<Column>(
            columns_to_be_copied.at(col)
        );
    }
}

Table::Table(size_t number_of_columns) : number_of_columns(number_of_columns){
    columns.resize(number_of_columns);
    for(size_t i = 0; i < number_of_columns; ++i){
        columns.at(i) = make_unique<Column>();
    }
    number_of_rows = 0;
}

Table::Table(shared_ptr<Table> table_to_copy){
    number_of_rows = table_to_copy->row_count();
    number_of_columns = table_to_copy->col_count();

    // Allocate the columns
    columns.resize(number_of_columns);
    for(size_t i = 0; i < number_of_columns; ++i){
        columns.at(i) = make_unique<Column>();
    }
    // Copy the columns from one table to the other
    for (size_t col_index = 0; col_index < table_to_copy->col_count(); col_index++)
        columns.at(col_index) = make_unique<Column>(
            *(table_to_copy->columns.at(col_index).get())
        );
}

Table::~Table(){}

vector<float> Table::materialize_row(size_t row_index){
    vector<float> row(col_count());
    for(size_t col = 0; col < col_count(); col++){
        row.at(col) = columns.at(col)->at(row_index);
    }
    return row;
}

void Table::append(vector<float> row){
    assert(row.size() == number_of_columns);
    for(size_t col = 0; col < row.size(); col++){
        columns.at(col)->append(row.at(col));
    }
    number_of_rows++;
}

void Table::exchange(size_t index1, size_t index2){
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
// Returns in which position the list is greater or equal to key
// In case of repetition, returns the first one, and all the others come
// right after it
size_t Table::CrackTable(size_t low, size_t high, float element, size_t c)
{
    size_t x1 = low;
    size_t x2 = high;

    while (x1 < x2)
    {
        if (columns.at(c)->at(x1) < element)
            x1++;
        else
        {
            while (x2 > x1 && (columns.at(c)->at(x2) >= element))
                x2--;
            if (x1 < x2)
            {
                exchange(x1, x2);
                x1++;
                x2--;
            }
        }
    }
    return x1;
}

// Cracks table in three from position i = low until i == high
// on column (c) with left key and right key
// Returns a pair of positions indicating where each patition starts
pair<size_t, size_t> Table::CrackTableInThree(size_t low, size_t high, float key_left, float key_right, size_t c)
{
    auto x1 = low;
    auto x2 = high;
    // "Eliminate" all the correct ones on the right side
    while (x2 > x1 && columns.at(c)->at(x2)  >= key_right)
        x2--;
    auto x3 = x2;
    // Fixes the middle partition and the last one
    // Stop when find someone from the first partition
    while (x3 > x1 && columns.at(c)->at(x3)  >= key_left)
    {
        if (columns.at(c)->at(x3) >= key_right)
        {
            exchange(x2, x3);
            x2--;
        }
        x3--;
    }
    // X3 stops when it finds someone from the first partition
    while (x1 < x3)
    {
        // Find element not from first partition
        if (columns.at(c)->at(x1)  < key_left)
            x1++;
        else
        {
            exchange(x1, x3);
            // Fixes the middle partition and the last one
            // Stop when find someone from the first partition
            while (x3 > x1 && columns.at(c)->at(x3)  >= key_left)
            {
                if (columns.at(c)->at(x3)  >= key_right)
                {
                    exchange(x2, x3);
                    x2--;
                }
                x3--;
            }
        }
    }

    return make_pair(x1,x2);
}

size_t Table::row_count(){
    return number_of_rows;
}

size_t Table::col_count(){
    return number_of_columns;
}
