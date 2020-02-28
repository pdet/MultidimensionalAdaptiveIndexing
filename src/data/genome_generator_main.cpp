#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

// Generators
#include "genome_generator.hpp"

#define FEATUREVECTORS_FILE "data/datasets/chr22_feature.vectors"
#define GENES_FILE "data/datasets/genes.txt"

#define DATA_FILE "data"
#define QUERY_FILE "queries"

void usage(){
    std::cout << std::endl;
    std::cout << "Usage:" <<std::endl;
    std::cout << "-r <number_of_rows>" << std::endl;
    std::cout << "-q <number_of_queries>" << std::endl;
    std::cout << "-t <query_type>" << std::endl;
}

int main(int argc, char* argv[]) {
    int64_t n_of_rows = -1;
    int64_t number_of_queries = -1;
    int64_t query_type = -1;

    int c;
    while ((c = getopt (argc, argv, "r:t:q:")) != -1){
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
            default:
                usage();
                exit(-1);
        }
    }

    if(n_of_rows == -1){
        std::cout << "Errors:" << std::endl;
        std::cout << "-r <n_of_rows> required" << std::endl;
        usage();
        exit(-1);
    }
    if(number_of_queries == -1){
        std::cout << "Errors:" << std::endl;
        std::cout << "-q <number_of_queries> required" << std::endl;
        usage();
        exit(-1);
    }
    if(query_type == -1){
        usage();
        exit(-1);
    }

    std::cout << "INFO\n";
    std::cout << n_of_rows << " rows\n";
    std::cout << 19 << " dimensions\n";
    std::cout << number_of_queries << " number of queries\n";
    std::cout << query_type << " query type\n";
    
    auto generator = GenomeGenerator(
            n_of_rows,
            number_of_queries,
            query_type,
            FEATUREVECTORS_FILE,
            GENES_FILE
        );
        generator.generate(DATA_FILE, QUERY_FILE);
    return 0;
}
