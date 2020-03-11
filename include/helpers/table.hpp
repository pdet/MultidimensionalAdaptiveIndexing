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

    int64_t row_count() const;

    int64_t col_count() const;
};

#endif // TABLE_H
