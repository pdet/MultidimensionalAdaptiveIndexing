#include "index_factory.hpp"
#include "data/data_reader.cpp"
#include <string>
#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv){
    string workload_path = "workload";
    string data_path = "data";
    string index_algorithm = "Full Scan";
    string sqlite_path = "results";

    int c;
    while ((c = getopt (argc, argv, "w:d:i:")) != -1)
        switch (c)
        {
        case 'w':
            workload_path = optarg;
            break;
        case 'd':
            data_path = optarg;
            break;
        case 'i':
            index_algorithm = optarg;
            break;
        default:
            cout << "Usage: -w <workload_path> -d <data_path> -i <algorithm>";
            return -1;
        }

    auto index = IndexFactory::getIndex(index_algorithm);

    auto table = DataReader::read_table(data_path);
    auto workload = DataReader::read_workload(workload_path);

    index->initialize(table);
    for(size_t i = 0; i < workload.size(); ++i){
        index->adapt_index(workload.at(i));
        index->range_query(workload.at(i));
    }
    return 0;
}