#ifndef TABLE_H
#define TABLE_H

#include "column.hpp"
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

    Table(vector<vector<float> > &columns_to_be_copied);

    Table(size_t number_of_columns);

    Table(shared_ptr<Table> table_to_copy);

    ~Table();

    vector<float> materialize_row(size_t row_index);

    void append(vector<float> row);

    void exchange(size_t index1, size_t index2);

    // Cracks table from position i = low until i == high
    // on column (c) with key (element)
    // Returns in which position the list is greater or equal to key
    // In case of repetition, returns the first one, and all the others come
    // right after it
    size_t CrackTable(size_t low, size_t high, float element, size_t c);

    size_t row_count();

    size_t col_count();
};

#endif // TABLE_H