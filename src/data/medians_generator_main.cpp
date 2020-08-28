#include "medians_generator.hpp"
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
    std::cout << "-p <partition_size>" << std::endl;
    std::cout << "-f <where_to_save_data_file>" << std::endl;
    std::cout << "-w <where_to_save_workload_file>" << std::endl;
}

int main(int argc, char* argv[]) {
    int64_t n_of_rows = -1;
    int64_t dimensions = -1;
    int64_t partition_size = -1;
    string data_path {""};
    string workload_path {""};

    int c;
    while ((c = getopt (argc, argv, "r:d:p:f:w:")) != -1){
        switch (c)
        {
            case 'r':
                n_of_rows = atoi(optarg);
                break;
            case 'd':
                dimensions = atoi(optarg);
                break;
            case 'p':
                partition_size = atoi(optarg);
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
    if(partition_size == -1){
        std::cout << "-p <partition_size> required" << std::endl;
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
    std::cout << partition_size << " partition size\n";
    std::cout << data_path << " will save data here\n";
    std::cout << workload_path << " will save workload here\n";

    auto generator = MediansGenerator(
            n_of_rows,
            dimensions,
            partition_size
            );
    generator.generate(data_path, workload_path);
    return 0;
}
