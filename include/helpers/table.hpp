#ifndef TABLE_H
#define TABLE_H

#include "column.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include <memory>

class Table{
    private:
        size_t number_of_rows;
        size_t number_of_columns;
    public:
        std::vector<std::unique_ptr<Column>> columns;

        Table(int64_t number_of_columns);

        // this is used when the first query fully copies the data (e.g., Quasii, Cracking KD-Tree)
        Table(Table *table_to_copy);

        // This just allocates the memory, no copy is done (e.g., PI)
        Table(size_t number_of_columns, size_t number_of_rows);

        ~Table();

        // File handling methods
        static std::unique_ptr<Table> read_file(std::string path);

        void save_file(std::string path);

        // Cracking methods
        std::pair<int64_t, int64_t> CrackTableInThree(int64_t low, int64_t high, float key_left, float key_right, int64_t c);

        int64_t CrackTable(int64_t low, int64_t high, float element, int64_t c);

        std::unique_ptr<float[]> materialize_row(int64_t row_index);

        void append(float* row);

        size_t row_count() const;

        size_t col_count() const;

        void exchange(size_t index1, size_t index2);
};

#endif // TABLE_H
