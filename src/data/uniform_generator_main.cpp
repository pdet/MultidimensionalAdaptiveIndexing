#include "uniform_generator.hpp"
#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA_FILE "data"
#define QUERY_FILE "queries"

void usage(){
    std::cout << std::endl;
    std::cout << "Usage:" <<std::endl;
    std::cout << "-r <number_of_rows>" << std::endl;
    std::cout << "-d <number_of_dimensions>" << std::endl;
    std::cout << "-s <selectivity>" << std::endl;
    std::cout << "-q <number_of_queries>" << std::endl;
}

int main(int argc, char* argv[]) {
    int64_t n_of_rows = -1;
    int64_t dimensions = -1;
    float selectivity = -1;
    int64_t number_of_queries = -1;

    int c;
    while ((c = getopt (argc, argv, "r:d:s:q:")) != -1){
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

    if(error){
        usage();
        exit(-1);
    }

    std::cout << "INFO\n";
    std::cout << n_of_rows << " rows\n";
    std::cout << dimensions << " dimensions\n";
    std::cout << selectivity << " selectivity\n";
    std::cout << number_of_queries << " number of queries\n";

    auto generator = UniformGenerator(
            n_of_rows,
            dimensions,
            selectivity,
            number_of_queries
            );
    generator.generate(DATA_FILE, QUERY_FILE);
    return 0;
}
