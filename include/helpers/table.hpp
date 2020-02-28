#ifndef TABLE_H
#define TABLE_H

#include "column.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include <memory>
#include <cassert>

using namespace std;

class Table{
private:
    int64_t number_of_rows;
    int64_t number_of_columns;
public:
    vector<unique_ptr<Column>> columns;

    Table(vector<vector<float> > &columns_to_be_copied);

    Table(int64_t number_of_columns);

    Table(Table *table_to_copy);

    Table(const Table &other);

    ~Table();

    static unique_ptr<Table> read_file(std::string path);

    void save_file(std::string path);

    vector<float> materialize_row(int64_t row_index);

    void append_column(Column col);

    void append(vector<float> row);

    void exchange(int64_t index1, int64_t index2);

    // Cracks table from position i = low until i == high
    // on column (c) with key (element)
    // Returns in which position the list is greater or equal to key
    // In case of repetition, returns the first one, and all the others come
    // right after it
    int64_t CrackTable(int64_t low, int64_t high, float element, int64_t c);

    // Cracks table in three from position i = low until i == high
    // on column (c) with left key and right key
    // Returns a pair of positions indicating where each patition starts
    pair<int64_t, int64_t> CrackTableInThree(int64_t low, int64_t high, float key_left, float key_right, int64_t c);

    int64_t row_count() const;

    int64_t col_count() const;
};

#endif // TABLE_H
