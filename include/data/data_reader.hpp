#ifndef DATA_READER_H
#define DATA_READER_H

#include "table.hpp"
#include "query.hpp"
#include <string>
#include <vector>

class DataReader
{
    public:
        static std::unique_ptr<Table> read_table(const std::string &data_path);
        static std::vector<Query> read_workload(const std::string &workload_path);
    private:
        static std::vector<float> split(const std::string& s, char delimiter);
};

#endif // DATA_READER_H
