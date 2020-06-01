#include "sequential_generator.hpp"
#include <random>
#include <vector>
#include <math.h>


SequentialGenerator::SequentialGenerator(
    size_t n_rows_, size_t n_dimensions_,
    float selectivity_, size_t n_queries_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_)
{}

unique_ptr<Table> SequentialGenerator::generate_table(){
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
unique_ptr<Workload> SequentialGenerator::generate_workload(){
    // Generate Queries
    // |        .---.
    // |        |   |
    // |    +---+---+
    // |    |   |
    // |----+---+
    // |    |
    // |--------------
    
    auto workload = make_unique<Workload>();
    float per_column_selectivity = std::pow(selectivity, 1.0/n_dimensions);

    // Gives me the square's side size
    float side = n_rows * per_column_selectivity;
    float half_side = side/2.0;

    // First square goes as close as possible to the origin
    auto center = half_side;

    for(size_t i = 0; i < n_queries && center <= n_rows - side; ++i, center += side){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            lows.at(j) = center - half_side;
            highs.at(j) = center + half_side;
            cols.at(j) = j;
        }
        workload->append(
            Query(lows, highs, cols)
        );
    }
    return workload;
}
