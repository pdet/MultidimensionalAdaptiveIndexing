#include "sequential_zoom_generator.hpp"
#include <random>
#include <vector>
#include <math.h>
#include <algorithm>    // std::reverse


SequentialZoomGenerator::SequentialZoomGenerator(
        size_t n_rows_, size_t n_dimensions_,
        float selectivity_, size_t n_queries_,
        bool out_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_),
    out(out_)
{}

unique_ptr<Table> SequentialZoomGenerator::generate_table(){
    // Generate Data
    auto table = make_unique<Table>(n_dimensions);
    std::mt19937 generator(1);
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

unique_ptr<Workload> SequentialZoomGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();
    float per_column_selectivity = std::pow(selectivity, 1.0/n_dimensions);

    float half_side = (n_rows * per_column_selectivity)/2.0;


    auto number_of_blocks = n_rows/(2*half_side);
    size_t n_queries_per_block = n_queries/number_of_blocks;
    float step = half_side / n_queries_per_block;

    for(size_t i = 0; i < number_of_blocks; ++i){
        float center = 2 * half_side * (i + 1);
        for(size_t s = 0 ; s < n_queries_per_block; ++s){
            std::vector<float> lows(n_dimensions);
            std::vector<float> highs(n_dimensions);
            std::vector<size_t> cols(n_dimensions);

            for(size_t j = 0; j < n_dimensions; ++j){
                lows.at(j) = (center-half_side) + s*step;
                highs.at(j) = (center+half_side) - s*step;
                cols.at(j) = j;
            }
            workload->append(
                    Query(lows, highs, cols)
                    );
        }
    }

    if(out){
        std::reverse(workload->queries.begin(), workload->queries.end());
    }
    return workload;
}
