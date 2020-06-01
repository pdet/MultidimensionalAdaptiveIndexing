#include "skewed_generator.hpp"
#include <random>
#include <vector>
#include <math.h>


SkewedGenerator::SkewedGenerator(
    size_t n_rows_, size_t n_dimensions_,
    float selectivity_, size_t n_queries_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_)
{
}

unique_ptr<Table> SkewedGenerator::generate_table(){
    // Generate Data
    auto table = make_unique<Table>(n_dimensions);
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

    return table;
}

unique_ptr<Workload> SkewedGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();
    float per_column_selectivity = std::pow(selectivity, 1.0/n_dimensions);

    double mean = n_rows/2.0;
    double std_dev = mean/8.0;

    std::mt19937 generator_query(1);
    std::normal_distribution<double> distr_query(mean, std_dev);

    for(size_t i = 0; i < n_queries; ++i){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            auto v = distr_query(generator_query);
            lows.at(j) = v - (per_column_selectivity/2.0 * n_rows);
            highs.at(j) = v + (per_column_selectivity/2.0 * n_rows);
            cols.at(j) = j;
        }
        workload->append(
            Query(lows, highs, cols)
        );
    }

    return workload;
}
