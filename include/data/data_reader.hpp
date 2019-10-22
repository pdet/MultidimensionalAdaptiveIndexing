#ifndef DATA_READER_H
#define DATA_READER_H

#include "table.hpp"
#include "query.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

class DataReader
{
    public:
        static std::unique_ptr<Table> read_table(const std::string &data_path);
        static std::vector<Query> read_workload(const std::string &workload_path);
    private:
        template <class T>
        static vector<T> split(const string& s, char delimiter)
        {
            string buf;                         // Have a buffer string
            stringstream ss(s.c_str());         // Insert the string into a stream

            vector<T> tokens;               // Create vector to hold our words

            while (ss >> buf)
                tokens.push_back(atof(buf.c_str()));

            return tokens;
        }
};

#endif // DATA_READER_H
