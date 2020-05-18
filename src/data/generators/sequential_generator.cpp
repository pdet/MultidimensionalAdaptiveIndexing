#include "sequential_generator.hpp"
#include <random>
#include <vector>
#include <math.h>


SequentialGenerator::SequentialGenerator(
    size_t n_rows_, size_t n_dimensions_,
    float selectivity_, size_t n_queries_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_)
{
    table = make_unique<Table>(n_dimensions);
    workload = make_unique<Workload>();
}

void SequentialGenerator::generate(std::string table_path, std::string query_path){
    // Generate Data
    std::mt19937 generator(0);
    std::uniform_int_distribution<int> distr(0, n_rows);

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

    auto center = n_rows * per_column_selectivity/2;

    for(
        size_t i = 0;
        i < n_queries && center < n_rows - (n_rows * per_column_selectivity);
        ++i, center += n_rows * per_column_selectivity
    ){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            lows.at(j) = center - (n_rows * per_column_selectivity/2);
            highs.at(j) = center + (n_rows * per_column_selectivity/2);
            cols.at(j) = j;
        }
        workload->append(
            Query(lows, highs, cols)
        );
    }

    workload->save_file(query_path);
}
