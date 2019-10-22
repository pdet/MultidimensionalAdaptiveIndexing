#include "my_generator.hpp"

#include <fstream>
#include <random>
#include <vector>
#include <math.h>

MyGenerator::MyGenerator(
                int64_t n_of_rows_,
                int64_t dimensions_,
                float selectivity_,
                int64_t number_of_queries_
                )
:               n_of_rows(n_of_rows_),
                dimensions(dimensions_),
                selectivity(selectivity_),
                number_of_queries(number_of_queries_){}

void MyGenerator::generate(
        const std::string &table_path,
        const std::string &workload_path
){
    // Generate Data
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(0, n_of_rows);

    std::ofstream table_file(table_path);

    for(int64_t i = 0; i < n_of_rows; ++i){
        for(int64_t j = 0; j < dimensions; ++j){
           table_file << distr(generator); 
           if(j + 1 != dimensions)
               table_file << " ";

        }
        table_file << std::endl;
    }

    table_file.close();

    // Generator Queries
    float per_column_selectivity = std::pow(selectivity, 1.0/dimensions);

    std::random_device rand_dev_query;
    std::mt19937 generator_query(rand_dev_query());
    std::uniform_int_distribution<int> distr_query(
        0, n_of_rows*(1-per_column_selectivity)
    );


    std::ofstream query_file(workload_path);

    for(int64_t i = 0; i < number_of_queries; ++i){
        std::vector<float> lows(dimensions);

        for(int64_t j = 0; j < dimensions; ++j)
            lows.at(j) = distr_query(generator_query);

        // Write the lows
        for(int64_t j = 0; j < dimensions; ++j){
           query_file << lows.at(j); 
           if(j + 1 != dimensions)
               query_file << " ";

        }
        query_file << std::endl;

        // Write the highs
        for(int64_t j = 0; j < dimensions; ++j){
           query_file << lows.at(j) + (int64_t)(n_of_rows * per_column_selectivity); 
           if(j + 1 != dimensions)
               query_file << " ";

        }
        query_file << std::endl;

        // Write the cols
         for(int64_t j = 0; j < dimensions; ++j){
           query_file << j;
           if(j + 1 != dimensions)
               query_file << " ";

        }
        query_file << std::endl;
    }

    query_file.close();

} 
