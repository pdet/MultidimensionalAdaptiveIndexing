#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

// Generators
#include "generator.hpp"
#include "my_generator.hpp"

#define FEATUREVECTORS_FILE "data/datasets/chr22_feature.vectors"
#define GENES_FILE "data/datasets/genes.txt"
#define POWER_DATASET_FILE "data/datasets/DEBS2012-ChallengeData.txt"

#define DATA_FILE "data"
#define QUERY_FILE "queries"

void usage(){
    std::cout << std::endl;
    std::cout << "Usage:" <<std::endl;
    std::cout << "-r <number_of_rows>" << std::endl;
    std::cout << "-d <number_of_dimensions>" << std::endl;
    std::cout << "-w <workload_choice>" << std::endl;
    std::cout << "  '-(0=normal, 1=clustered, 2=uniform, 3=GMRQB, 4=power)" << std::endl;
    std::cout << "  '- GMRQB has fixed selectivity and 19 dimensions." << std::endl;
    std::cout << "  '- power has fixed 4 dimensions." << std::endl;
    std::cout << "-s <selectivity>" << std::endl;
    std::cout << "-q <number_of_queries>" << std::endl;
    std::cout << "-t <query_type>" << std::endl;
}

int main(int argc, char* argv[]) {
    int64_t n_of_rows = -1;
    int64_t dimensions = -1;
    int64_t workload = -1;
    float selectivity = -1;
    int64_t number_of_queries = -1;
    int64_t query_type = -1;

    int c;
    while ((c = getopt (argc, argv, "r:d:s:w:t:q:")) != -1){
        switch (c)
        {
            case 'w':
                workload = atoi(optarg);
                break;
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
    if(dimensions == -1){
        std::cout << "Errors:" << std::endl;
        std::cout << "-d <dimensions> required" << std::endl;
        usage();
        exit(-1);
    }
    if(workload == -1){
        std::cout << "Errors:" << std::endl;
        std::cout << "-w <workload> required" << std::endl;
        usage();
        exit(-1);
    }
    if(selectivity == -1){
        std::cout << "Errors:" << std::endl;
        std::cout << "-s <selectivity> required" << std::endl;
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
        query_type = 7;
        usage();
        exit(-1);
    }

    if(workload == 3)
        std::cout << "INFO: " << n_of_rows << " vectors, " << 19 << " dimensions." << std::endl;
    else if(workload == 4)
        std::cout << "INFO: " << n_of_rows << " vectors, " << 4 << " dimensions." << std::endl;
    else
        std::cout << "INFO: " << n_of_rows << " vectors, " << dimensions << " dimensions." << std::endl;

    if(workload == 5){
        auto generator = MyGenerator(
            n_of_rows,
            dimensions,
            selectivity,
            number_of_queries
        );
        generator.generate(DATA_FILE, QUERY_FILE);
    }else{
        auto generator = Generator(

            n_of_rows,
            dimensions,
            workload,
            selectivity,
            number_of_queries,
            query_type,
            POWER_DATASET_FILE,
            FEATUREVECTORS_FILE,
            GENES_FILE
        );
        generator.generate(DATA_FILE, QUERY_FILE);
    }
    return 0;
}
