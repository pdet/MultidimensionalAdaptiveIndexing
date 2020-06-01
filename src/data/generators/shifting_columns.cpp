#include "shifting_columns.hpp"
#include <random>
#include <vector>
#include <math.h>
#include <limits>


ShiftingColumnsGenerator::ShiftingColumnsGenerator(
        size_t n_rows_, size_t n_dimensions_,
        float selectivity_, size_t n_queries_
        ) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_)
{
}

unique_ptr<Table> ShiftingColumnsGenerator::generate_table(){
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

unique_ptr<Workload> ShiftingColumnsGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();
    float per_column_selectivity = std::pow(selectivity, 1.0/n_dimensions);

    std::mt19937 generator_query(1);
    std::uniform_int_distribution<int> distr_query(
            0, n_rows*(1-per_column_selectivity)
            );

    size_t shift_col = 1;
    size_t window = 3;
    size_t half_window = window / 2;

    for(size_t i = 0; i < n_queries; ++i){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            cols.at(j) = j;
            if((shift_col - half_window) <= j && j <= (shift_col + half_window)){
                lows.at(j) = distr_query(generator_query);
                highs.at(j) = lows.at(j) + (int64_t)(n_rows * per_column_selectivity);
            }else{
                lows.at(j) = numeric_limits<float>::lowest();
                highs.at(j) = numeric_limits<float>::max();
            }
        }
        if((i+1) % 10 == 0){
            shift_col += window;
            shift_col %= n_dimensions;
        }

        workload->append(
                Query(lows, highs, cols)
                );
    }

    return workload;
}
