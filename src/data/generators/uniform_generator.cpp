#include "uniform_generator.hpp"
#include <random>
#include <vector>
#include <math.h>


UniformGenerator::UniformGenerator(
    size_t n_rows_, size_t n_dimensions_,
    float selectivity_, size_t n_queries_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_)
{
    table = make_unique<Table>(n_dimensions);
    workload = make_unique<Workload>();
}

void UniformGenerator::generate(std::string table_path, std::string query_path){
    // Generate Data
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<float> distr(0, n_rows);

    for(size_t i = 0; i < n_rows; ++i){
        float* row = new float[n_dimensions];
        for(size_t j = 0; j < n_dimensions; ++j){
           row[j] = distr(generator); 
        }
        table->append(row);
        delete[] row;
    }

    table->save_file(table_path);

    // Generator Queries
    float per_column_selectivity = std::pow(selectivity, 1.0/n_dimensions);

    std::random_device rand_dev_query;
    std::mt19937 generator_query(rand_dev_query());
    std::uniform_int_distribution<float> distr_query(
        0, n_rows*(1-per_column_selectivity)
    );

    for(size_t i = 0; i < n_queries; ++i){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            lows.at(j) = distr_query(generator_query);
            highs.at(j) = lows.at(j) + (int64_t)(n_rows * per_column_selectivity);
            cols.at(j) = j;
        }
        workload->append(
            Query(lows, highs, cols)
        );
    }

    workload->save_file(query_path);
}
