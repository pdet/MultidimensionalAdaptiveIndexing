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
    string results_file;
    size_t algorithm_id = 0;
    int number_of_repetitions = 3;
    map<string, string> config;

    int c;
    while ((c = getopt (argc, argv, "w:d:i:r:s:p:a:t:c")) != -1)
        switch (c)
        {
        case 'w':
            workload_path = optarg;
            break;
        case 'd':
            data_path = optarg;
            break;
        case 'i':
            algorithm_id = atoi(optarg);
            break;
        case 'r':
            number_of_repetitions = atoi(optarg);
            break;
        case 's':
            results_file = optarg;
            break;
        case 'p':
            config["minimum_partition_size"] = optarg;
            break;
        case 'a':
            config["delta"] = optarg;
            break;
        case 't':
            config["workload_adaptive"] = '1';
            break;
            case 'c':
                config["interactivity_threshold"] = '1';
                break;
        default:
            cout << "Usage:\n";
            cout << "-w <workload_path>\n";
            cout << "-d <data_path>\n";
            cout << "-i <algorithm id>\n";
            cout << "-r <number_of_repetitions>\n";
            cout << "-s <file_to_save_results>\n";
            cout << "-p <minimum_partition_size>\n";
            cout << "-a <delta>\n";
            cout << "-t <flag for progressive index to turn adaptive>\n";
            cout << "-c <flag for progressive index to use costmodels>\n";
            return -1;
        }

    for(auto repetition = 0; repetition < number_of_repetitions; repetition++){
        auto index = IndexFactory::getIndex(algorithm_id, config);

        cout << endl;
        cout << index->name() << endl;
        cout << "Repetition: " << repetition << endl;
        cout << "Workload: " << workload_path << endl;
        cout << "Data: " << data_path << endl;
        cout << "Configurations:" << endl;

        for(auto& element : config){
            cout << '\t' << element.first << ": " << element.second << endl;
        }

        auto table = Table::read_file(data_path);
        auto workload = Workload::read_file(workload_path);

        index->initialize(table.get());
        for(size_t i = 0; i < workload.query_count(); ++i){
            index->adapt_index(workload.queries.at(i));
            index->range_query(workload.queries.at(i));
            //index->draw_index(results_path + '/' + index->name() + ".dot");
        }

        index->measurements->save(
                results_file,
                repetition
                );
    }
    return 0;
}
