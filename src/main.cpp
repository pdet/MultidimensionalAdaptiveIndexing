#include "index_factory.hpp"
#include "data_reader.hpp"
#include <string>
#include <iostream>
#include <map>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

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
        case 'i':
            index_algorithm = optarg;
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
            cout << "-i <algorithm>\n";
            cout << "-r <number_of_repetitions>\n";
            cout << "-s <folder_to_save_results>\n";
            cout << "-p <minimum_partition_size>\n";
            return -1;
        }

    for(auto repetition = 0; repetition < number_of_repetitions; repetition++){
        auto index = IndexFactory::getIndex(index_algorithm, config);

        cout << index->name() << " Repetition: " << repetition << endl;
        
        std::system(("mkdir " + results_path + "/" + '"' + index->name() + '"').c_str());

        auto table = DataReader::read_table(data_path);
        auto workload = DataReader::read_workload(workload_path);

        index->initialize(table.get());
        for(size_t i = 0; i < workload.size(); ++i){
            index->adapt_index(workload.at(i));
            index->draw_index(
                results_path + "/" + index->name() + "/" + std::to_string(i) + ".dot"
            );
            index->range_query(workload.at(i));
        }

        index->measurements->save(results_path + "/results.csv", repetition, index->name());
    }
    return 0;
}
