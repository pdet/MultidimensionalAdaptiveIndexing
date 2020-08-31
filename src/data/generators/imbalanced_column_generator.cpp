#include "imbalanced_column_generator.hpp"
#include <random>
#include <vector>
#include <math.h>
#include <algorithm>

ImbalancedColumnGenerator::ImbalancedColumnGenerator(
        size_t n_rows_, size_t n_queries_, bool ascending_
) : n_rows(n_rows_), n_dimensions(10),
    n_queries(n_queries_), ascending(ascending_)
{
}

unique_ptr<Table> ImbalancedColumnGenerator::generate_table(){
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

unique_ptr<Workload> ImbalancedColumnGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();
    std::vector<float>selectivities = {1, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
    if(ascending){
       std::reverse(selectivities.begin(), selectivities.end());
    }

    std::mt19937 generator_query(1);
    std::uniform_int_distribution<int> distr_query(
            0, n_rows
    );

    for(size_t i = 0; i < n_queries; ++i){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            auto per_column_selectivity = selectivities.at(j);
            if((int64_t) (n_rows - (n_rows * per_column_selectivity)) == 0){
                lows.at(j) = 0;
            }else{
                lows.at(j) = distr_query(generator_query) % (int64_t) (n_rows - (n_rows * per_column_selectivity));
            }
            highs.at(j) = lows.at(j) + (int64_t)(n_rows * per_column_selectivity);
            cols.at(j) = j;
        }
        workload->append(
                Query(lows, highs, cols)
        );
    }

    return workload;
}
