#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

// Generators
#include "power_generator.hpp"

void usage(){
    std::cout << std::endl;
    std::cout << "Usage:" <<std::endl;
    std::cout << "-r <number_of_rows>" << std::endl;
    std::cout << "-q <number_of_queries>" << std::endl;
    std::cout << "-p <full path to power dataset file>" << std::endl;
    std::cout << "-f <where_to_save_data_file>" << std::endl;
    std::cout << "-w <where_to_save_workload_file>" << std::endl;
}

int main(int argc, char* argv[]) {
    int64_t n_of_rows = -1;
    int64_t number_of_queries = -1;
    int64_t query_type = -1;
    string power_dataset_file {""};
    string data_path {""};
    string workload_path {""};

    int c;
    while ((c = getopt (argc, argv, "r:t:q:p:f:w:")) != -1){
        switch (c)
        {
            case 'r':
                n_of_rows = atoi(optarg);
                break;
            case 'q':
                number_of_queries = atoi(optarg);
                break;
            case 't':
                query_type = atoi(optarg);
                break;
            case 'p':
                power_dataset_file = optarg;
                break;
            case 'f':
                data_path = optarg;
                break;
            case 'w':
                workload_path = optarg;
                break;

            default:
                usage();
                exit(-1);
        }
    }

    bool error = false;

    if(n_of_rows == -1){
        std::cout << "-r <n_of_rows> required" << std::endl;
        error = true;
    }
    if(number_of_queries == -1){
        std::cout << "-q <number_of_queries> required" << std::endl;
        error = true;
    }

    if(power_dataset_file == ""){
        std::cout << "-p <full path to power dataset file> required" << std::endl;
        error = true;
    }

    if(data_path == ""){
        std::cout << "-f <where_to_save_data_file> required" << std::endl;
        error = true;
    }
    if(workload_path == ""){
        std::cout << "-w <where_to_save_workload_file> required" << std::endl;
        error = true;
    }


    if(error){
        usage();
        exit(-1);
    }

    std::cout << "INFO\n";
    std::cout << n_of_rows << " rows\n";
    std::cout << 3 << " dimensions\n";
    std::cout << number_of_queries << " number of queries\n";
    std::cout << data_path << " will save data here\n";
    std::cout << workload_path << " will save workload here\n";


    auto generator = PowerGenerator(
            n_of_rows,
            number_of_queries,
            power_dataset_file 
            );
    generator.generate(data_path, workload_path);
    return 0;
}
