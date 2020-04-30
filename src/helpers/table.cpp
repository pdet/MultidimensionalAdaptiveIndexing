#include "table.hpp"
#include "column.hpp"
#include <cstdint>
#include <iostream>
#include <fstream>
#include "split_string.hpp"

using namespace std;

Table::Table(size_t number_of_columns){
    columns.resize(number_of_columns);
    for(size_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>();
    }
}

Table::Table(size_t number_of_columns, size_t number_of_rows)
{
    columns.resize(number_of_columns);
    for(size_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>(number_of_rows);
    }
}

Table::Table(Table *table_to_copy){
    auto number_of_rows = table_to_copy->row_count();
    auto number_of_columns = table_to_copy->col_count();

    // Allocate the columns
    columns.resize(number_of_columns);
    for(size_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>();
    }
    // Copy the columns from one table to the other
    for (size_t col_index = 0; col_index < table_to_copy->col_count(); col_index++){
        columns[col_index] = make_unique<Column>(
            table_to_copy->columns[col_index]->data, number_of_rows
        );
    }
}

Table::~Table(){}

std::unique_ptr<Table> Table::read_file(std::string path){
    ifstream file(path.c_str(), ios::in | ios::binary);
    if(!file.is_open()){
        cout << "Error *opening* data file\n";
        exit(-1);
    }

    // Read number of columns
    uint64_t n_cols;
    file.read(reinterpret_cast<char*>(&n_cols), sizeof(uint64_t));

    // Read number of rows
    uint64_t n_rows;
    file.read(reinterpret_cast<char*>(&n_rows), sizeof(uint64_t));

    auto table = make_unique<Table>(n_cols, n_rows);

    for(size_t i = 0; i < n_rows; ++i){
        float *row = new float[n_cols];
        file.read(reinterpret_cast<char*>(row), sizeof(float) * n_cols);
        table->append(row);
        delete[] row;
    }

    file.close();

    return table;

}

void Table::save_file(std::string path){
    ofstream file(path.c_str(), ios::out | ios::binary | ios::trunc);
    if(!file.is_open()){
        cout << "Error *opening* file\n";
        exit(-1);
    }

    // Write number of columns
    uint64_t n_cols = col_count();
    file.write(reinterpret_cast<const char*>(&n_cols), sizeof(uint64_t));

    // Write number of rows
    uint64_t n_rows = row_count();
    file.write(reinterpret_cast<const char*>(&n_rows), sizeof(uint64_t));
    for(size_t i = 0; i < n_rows; ++i){
        for(size_t j = 0; j < n_cols; ++j){
            file.write(reinterpret_cast<const char*>(&(columns[j]->data[i])), sizeof(float));
        }
    }
    file.close();
}

std::unique_ptr<float[]> Table::materialize_row(size_t row_index){
    auto row = make_unique<float[]>(row_count());
    for(size_t col = 0; col < col_count(); col++){
        row[col] = columns[col]->data[row_index];
    }
    return row;
}

void Table::append(float* row){
    for(size_t col = 0; col < col_count(); col++){
        columns[col]->append(row[col]);
    }
}


size_t Table::row_count() const{
    if(col_count() == 0)
        return 0;
    return columns[0]->size;
}

size_t Table::col_count() const{
    return columns.size();
}

size_t Table::CrackTable(size_t low, size_t high, float element, size_t c)
{
    int64_t x1 = low;
    int64_t x2 = high - 1;

    while (x1 <= x2 && x2 > 0)
    {
        if (columns[c]->data[x1] < element)
            x1++;
        else
        {
            while (x2 > 0 && x2 >= x1 && (columns[c]->data[x2] >= element))
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

pair<size_t, size_t> Table::CrackTableInThree(size_t low, size_t high, float key_left, float key_right, size_t c)
{
    auto p1 = CrackTable(low, high, key_left, c);
    auto p2 = CrackTable(p1, high, key_right, c);
    return make_pair(p1, p2);
}

void Table::exchange(size_t index1, size_t index2){
    for(size_t column_index = 0; column_index < col_count(); ++column_index){
        auto value1 = columns[column_index]->data[index1];
        auto value2 = columns[column_index]->data[index2];

        auto tmp = value1;
        columns[column_index]->data[index1] = value2;
        columns[column_index]->data[index2] = tmp;
    }
}
