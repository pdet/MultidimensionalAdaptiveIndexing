#include "index_factory.hpp"
#include "data_reader.hpp"
#include <string>
#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv){
    string workload_path = "queries";
    string data_path = "data";
    string index_algorithm = "Full Scan";
    string sqlite_path;
    int number_of_repetitions = 3;

    int c;
    while ((c = getopt (argc, argv, "w:d:i:r:s:")) != -1)
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
        case 'r':
            number_of_repetitions = atoi(optarg);
            break;
        case 's':
            sqlite_path = optarg;
            break;
        default:
            cout << "Usage: -w <workload_path> -d <data_path> -i <algorithm>";
            return -1;
        }

    for(auto repetition = 0; repetition < number_of_repetitions; repetition++){
        auto index = IndexFactory::getIndex(index_algorithm);

        auto table = DataReader::read_table(data_path);
        auto workload = DataReader::read_workload(workload_path);

        index->initialize(table.get());
        for(size_t i = 0; i < workload.size(); ++i){
            index->adapt_index(workload.at(i));
            index->range_query(workload.at(i));
        }

        index->measurements->save_to_sql(sqlite_path + "/results", repetition, index->name());
    }
    return 0;
}
