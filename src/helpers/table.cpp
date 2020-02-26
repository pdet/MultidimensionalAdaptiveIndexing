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
    for (int64_t col = 0; col < number_of_columns; col++){
        columns.at(col) = make_unique<Column>(
            columns_to_be_copied.at(col)
        );
    }
}

Table::Table(int64_t number_of_columns) : number_of_columns(number_of_columns){
    columns.resize(number_of_columns);
    for(int64_t i = 0; i < number_of_columns; ++i){
        columns.at(i) = make_unique<Column>();
    }
    number_of_rows = 0;
}

Table::Table(Table *table_to_copy){
    number_of_rows = table_to_copy->row_count();
    number_of_columns = table_to_copy->col_count();

    // Allocate the columns
    columns.resize(number_of_columns);
    for(int64_t i = 0; i < number_of_columns; ++i){
        columns.at(i) = make_unique<Column>();
    }
    // Copy the columns from one table to the other
    for (int64_t col_index = 0; col_index < table_to_copy->col_count(); col_index++)
        columns.at(col_index) = make_unique<Column>(
            *(table_to_copy->columns.at(col_index).get())
        );
}

Table::Table(const Table &other){
    number_of_rows = other.row_count();
    number_of_columns = other.col_count();

    // Allocate the columns
    columns.resize(number_of_columns);
    for(int64_t i = 0; i < number_of_columns; ++i){
        columns.at(i) = make_unique<Column>();
    }
    // Copy the columns from one table to the other
    for (int64_t col_index = 0; col_index < other.col_count(); col_index++)
        columns.at(col_index) = make_unique<Column>(
            *(other.columns.at(col_index).get())
        );
}

Table::~Table(){}
void Table::append_column(Column col){
    columns.push_back(make_unique<Column>(col));
    number_of_columns++;
}

vector<float> Table::materialize_row(int64_t row_index){
    vector<float> row(col_count());
    for(int64_t col = 0; col < col_count(); col++){
        row.at(col) = columns.at(col)->at(row_index);
    }
    return row;
}

void Table::append(vector<float> row){
    assert(row.size() == number_of_columns);
    for(int64_t col = 0; col < row.size(); col++){
        columns.at(col)->append(row.at(col));
    }
    number_of_rows++;
}

void Table::exchange(int64_t index1, int64_t index2){
    for(int64_t column_index = 0; column_index < number_of_columns; ++column_index){
        auto value1 = columns.at(column_index)->at(index1);
        auto value2 = columns.at(column_index)->at(index2);

        auto tmp = value1;
        columns.at(column_index)->assign(index1, value2);
        columns.at(column_index)->assign(index2, tmp);
    }
}

// Cracks table from position i = low until i < high
// on column (c) with key (element)
// Returns in which position the list is greater or equal to key
// In case of repetition, returns the first one, and all the others come
// right after it
int64_t Table::CrackTable(int64_t low, int64_t high, float element, int64_t c)
{
    int64_t x1 = low;
    int64_t x2 = high - 1;

    while (x1 <= x2 && x2 > 0)
    {
        if (columns.at(c)->at(x1) < element)
            x1++;
        else
        {
            while (x2 > 0 && x2 >= x1 && (columns.at(c)->at(x2) >= element))
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

// Cracks table in three from position i = low until i < high
// on column (c) with left key and right key
// Returns a pair of positions indicating where each patition starts
pair<int64_t, int64_t> Table::CrackTableInThree(int64_t low, int64_t high, float key_left, float key_right, int64_t c)
{
    auto p1 = CrackTable(low, high, key_left, c);
    auto p2 = CrackTable(p1, high, key_right, c);
    return make_pair(p1, p2);
}

int64_t Table::row_count() const{
    return number_of_rows;
}

int64_t Table::col_count() const{
    return number_of_columns;
}
