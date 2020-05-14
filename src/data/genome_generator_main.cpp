#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

// Generators
#include "genome_generator.hpp"

#define DATA_FILE "genome_data"
#define QUERY_FILE "genome_queries"

void usage(){
    std::cout << std::endl;
    std::cout << "Usage:" <<std::endl;
    std::cout << "-r <number_of_rows>" << std::endl;
    std::cout << "-q <number_of_queries>" << std::endl;
    std::cout << "-t <query_type>" << std::endl;
    std::cout << "-f <full path to features file>" << std::endl;
    std::cout << "-g <full path to genomes file>" << std::endl;
}

int main(int argc, char* argv[]) {
    int64_t n_of_rows = -1;
    int64_t number_of_queries = -1;
    int64_t query_type = -1;
    string features_file {""};
    string genomes_file {""};

    int c;
    while ((c = getopt (argc, argv, "r:t:q:f:g:")) != -1){
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
            case 'f':
                features_file = optarg;
                break;
            case 'g':
                genomes_file = optarg;
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
    if(query_type == -1){
        std::cout << "-t <query_type> required" << std::endl;
        error = true;
    }

    if(features_file == ""){
        std::cout << "-f <full path to features file> required" << std::endl;
        error = true;
    }

    if(genomes_file == ""){
        std::cout << "-g <full path to genomes file> required" << std::endl;
        error = true;
    }

    if(error){
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
            features_file,
            genomes_file
            );
    generator.generate(DATA_FILE, QUERY_FILE);
    return 0;
}
