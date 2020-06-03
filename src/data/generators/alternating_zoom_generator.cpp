#include "alternating_zoom_generator.hpp"
#include <random>
#include <vector>
#include <math.h>
#include <algorithm>    // std::reverse


AlternatingZoomGenerator::AlternatingZoomGenerator(
    size_t n_rows_, size_t n_dimensions_,
    float selectivity_, size_t n_queries_,
    bool out_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_),
    out(out_)
{}

unique_ptr<Table> AlternatingZoomGenerator::generate_table(){
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

unique_ptr<Workload> AlternatingZoomGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();
    float per_column_selectivity = std::pow(selectivity, 1.0/n_dimensions);

    auto center = n_rows/2.0;

    float half_side = (n_rows * per_column_selectivity)/2.0;

    size_t alternating_dim = 0;

    vector<float> factor (n_dimensions, 1);

    for(size_t i = n_queries; i > 0; --i, alternating_dim = (alternating_dim + 1) % n_dimensions){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            lows.at(j) = center - half_side * factor[j];
            highs.at(j) = center + half_side * factor[j];
            cols.at(j) = j;
        }

        factor[alternating_dim] -= factor[alternating_dim] * (half_side/n_queries);

        workload->append(
                Query(lows, highs, cols)
                );
    }

    if(out){
        std::reverse(workload->queries.begin(), workload->queries.end());
    }
    return workload;
}
