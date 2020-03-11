#include "column.hpp"
#include "table.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include "split_string.hpp"

using namespace std;
Table::Table(vector<vector<float> > &columns_to_be_copied){
    number_of_columns = columns_to_be_copied.size();
    number_of_rows = columns_to_be_copied[0].size();

    columns.resize(number_of_columns);
    for (int64_t col = 0; col < number_of_columns; col++){
        columns[col] = make_unique<Column>(
            columns_to_be_copied[col]
        );
    }
}

Table::Table(int64_t number_of_columns) : number_of_columns(number_of_columns){
    columns.resize(number_of_columns);
    for(int64_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>();
    }
    number_of_rows = 0;
}

Table::Table(Table *table_to_copy){
    number_of_rows = table_to_copy->row_count();
    number_of_columns = table_to_copy->col_count();

    // Allocate the columns
    columns.resize(number_of_columns);
    for(int64_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>();
    }
    // Copy the columns from one table to the other
    for (int64_t col_index = 0; col_index < table_to_copy->col_count(); col_index++)
        columns[col_index] = make_unique<Column>(
            *(table_to_copy->columns[col_index].get())
        );
}

Table::Table(const Table &other){
    number_of_rows = other.row_count();
    number_of_columns = other.col_count();

    // Allocate the columns
    columns.resize(number_of_columns);
    for(int64_t i = 0; i < number_of_columns; ++i){
        columns[i] = make_unique<Column>();
    }
    // Copy the columns from one table to the other
    for (int64_t col_index = 0; col_index < other.col_count(); col_index++)
        columns[col_index] = make_unique<Column>(
            *(other.columns[col_index].get())
        );
}

unique_ptr<Table> Table::read_file(std::string path){
    ifstream file(path.c_str(), ios::in);
    if(!file.is_open()){
        cout << "Error *opening* data file\n";
        exit(-1);
    }
    string line;
    if(!getline(file, line)){
        cout << "Error *reading* data file\n";
        exit(-1);
    }

    auto row = SplitString<float>::split(line, ' ');
    auto table = make_unique<Table>(row.size());
    table->append(row);

    while(getline(file,line)){
        row = SplitString<float>::split(line, ' ');
        table->append(row);
    }

    file.close();

    return table;

}

void Table::save_file(std::string path){
    ofstream file(path.c_str(), ios::out);
    if(!file.is_open()){
        cout << "Error *opening* file\n";
        exit(-1);
    }

    for(size_t i = 0; i < number_of_rows; ++i){
        auto row = materialize_row(i);
        for(size_t j = 0; j < number_of_columns - 1; ++j){
            file << row[j] << " "; 
        }
        file << row[number_of_columns-1] << "\n";
    }

    file.close();
}

Table::~Table(){}

void Table::append_column(Column col){
    columns.push_back(make_unique<Column>(col));
    number_of_columns++;
}

vector<float> Table::materialize_row(int64_t row_index){
    vector<float> row(col_count());
    for(int64_t col = 0; col < col_count(); col++){
        row[col] = columns[col]->data[row_index];
    }
    return row;
}

void Table::append(vector<float> row){
    assert(row.size() == number_of_columns);
    for(int64_t col = 0; col < row.size(); col++){
        columns[col]->append(row[col]);
    }
    number_of_rows++;
}


int64_t Table::row_count() const{
    return number_of_rows;
}

int64_t Table::col_count() const{
    return number_of_columns;
}
