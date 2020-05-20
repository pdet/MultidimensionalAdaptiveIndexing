#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

// Generators
#include "skyserver_generator.hpp"

void usage(){
    std::cout << std::endl;
    std::cout << "Usage:" <<std::endl;
    std::cout << "-p <full path to skyserver dataset file>" << std::endl;
    std::cout << "-q <full path to skyserver query file>" << std::endl;
    std::cout << "-f <where_to_save_data_file>" << std::endl;
    std::cout << "-w <where_to_save_workload_file>" << std::endl;
}

int main(int argc, char* argv[]) {
    string skyserver_dataset_file {""};
    string skyserver_query_file {""};
    string data_path {""};
    string workload_path {""};

    int c;
    while ((c = getopt (argc, argv, "r:t:q:p:f:w:")) != -1){
        switch (c)
        {
            case 'p':
                skyserver_dataset_file = optarg;
                break;
            case 'q':
                skyserver_query_file = optarg;
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

    if(skyserver_dataset_file.empty()){
        std::cout << "-p <full path to skyserver dataset file> required" << std::endl;
        error = true;
    }
    if(skyserver_dataset_file.empty()){
        std::cout << "-q <full path to skyserver query file> required" << std::endl;
        error = true;
    }
    if(data_path.empty()){
        std::cout << "-f <where_to_save_data_file> required" << std::endl;
        error = true;
    }
    if(workload_path.empty()){
        std::cout << "-w <where_to_save_workload_file> required" << std::endl;
        error = true;
    }


    if(error){
        usage();
        exit(-1);
    }

    std::cout << "INFO\n";
    std::cout << data_path << " will save data here\n";
    std::cout << workload_path << " will save workload here\n";


    auto generator = SkyserverGenerator(
            skyserver_dataset_file,
            skyserver_query_file
            );
    generator.generate(data_path, workload_path);
    return 0;
}
