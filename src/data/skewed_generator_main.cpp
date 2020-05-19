#include "skewed_generator.hpp"
#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

void usage(){
    std::cout << std::endl;
    std::cout << "Usage:" <<std::endl;
    std::cout << "-r <number_of_rows>" << std::endl;
    std::cout << "-d <number_of_dimensions>" << std::endl;
    std::cout << "-s <selectivity>" << std::endl;
    std::cout << "-q <number_of_queries>" << std::endl;
    std::cout << "-f <where_to_save_data_file>" << std::endl;
    std::cout << "-w <where_to_save_workload_file>" << std::endl;
}

int main(int argc, char* argv[]) {
    int64_t n_of_rows = -1;
    int64_t dimensions = -1;
    float selectivity = -1;
    int64_t number_of_queries = -1;
    string data_path {""};
    string workload_path {""};

    int c;
    while ((c = getopt (argc, argv, "r:d:s:q:f:w:m:v:")) != -1){
        switch (c)
        {
            case 'r':
                n_of_rows = atoi(optarg);
                break;
            case 'd':
                dimensions = atoi(optarg);
                break;
            case 's':
                selectivity = atof(optarg);
                break;
            case 'q':
                number_of_queries = atoi(optarg);
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
    if(dimensions == -1){
        std::cout << "-d <dimensions> required" << std::endl;
        error = true;
    }
    if(selectivity == -1){
        std::cout << "-s <selectivity> required" << std::endl;
        error = true;
    }
    if(number_of_queries == -1){
        std::cout << "-q <number_of_queries> required" << std::endl;
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
    std::cout << dimensions << " dimensions\n";
    std::cout << selectivity << " selectivity\n";
    std::cout << number_of_queries << " number of queries\n";
    std::cout << data_path << " will save data here\n";
    std::cout << workload_path << " will save workload here\n";

    auto generator = SkewedGenerator(
            n_of_rows,
            dimensions,
            selectivity,
            number_of_queries
            );
    generator.generate(data_path, workload_path);
    return 0;
}
