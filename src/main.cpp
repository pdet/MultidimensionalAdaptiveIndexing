#include "index_factory.hpp"
#include <string>
#include <iostream>
#include <map>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "table.hpp"
#include "workload.hpp"

int main(int argc, char** argv){
    string workload_path = "queries";
    string data_path = "data";
    string index_algorithm = "Full Scan";
    string results_path;
    int number_of_repetitions = 3;
    map<string, string> config;

    int c;
    while ((c = getopt (argc, argv, "w:d:i:r:s:p:")) != -1)
        switch (c)
        {
        case 'w':
            workload_path = optarg;
            break;
        case 'd':
            data_path = optarg;
            break;
        case 'r':
            number_of_repetitions = atoi(optarg);
            break;
        case 's':
            results_path = optarg;
            break;
        case 'p':
            config["minimum_partition_size"] = optarg;
            break;
        default:
            cout << "Usage:\n";
            cout << "-w <workload_path>\n";
            cout << "-d <data_path>\n";
            cout << "-r <number_of_repetitions>\n";
            cout << "-s <file_to_save_results>\n";
            cout << "-p <minimum_partition_size>\n";
            return -1;
        }

    for(auto& id : IndexFactory::algorithmIDs()){
        for(auto repetition = 0; repetition < number_of_repetitions; repetition++){
            auto index = IndexFactory::getIndex(id, config);

            cout << index->name() << " Repetition: " << repetition << endl;

            auto table = Table::read_file(data_path);
            auto workload = Workload::read_file(workload_path);

            index->initialize(table.get());
            for(size_t i = 0; i < workload.query_count(); ++i){
                index->adapt_index(workload.queries.at(i));
                index->range_query(workload.queries.at(i));
            }

            auto file_name = results_path + "/results";
            file_name += "-" + index->name();
            file_name += "-" + std::to_string(table->col_count());
            file_name += ".csv"; 

            index->measurements->save(
                file_name,
                repetition
            );
        }
    }
    return 0;
}
