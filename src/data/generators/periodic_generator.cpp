#include "periodic_generator.hpp"
#include <random>
#include <vector>
#include <math.h>


PeriodicGenerator::PeriodicGenerator(
    size_t n_rows_, size_t n_dimensions_,
    float selectivity_, size_t n_queries_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_)
{}

unique_ptr<Table> PeriodicGenerator::generate_table(){
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

unique_ptr<Workload> PeriodicGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();
    float per_column_selectivity = std::pow(selectivity, 1.0/n_dimensions);
    float half_side = (n_rows*per_column_selectivity) / 2;

    size_t reps = 4;

    for(size_t i = 0; i < n_rows/reps; ++i){
        vector<float> begin(n_dimensions);
        vector<float> end(n_dimensions);

        begin[0] = half_side;
        end[0] = n_rows - half_side;

        for(size_t d = 1; d < n_dimensions; ++d){
            begin[d] = i * (n_rows/reps) + half_side;
            end[d] = (i+1) * (n_rows/reps) - half_side;
        }

        auto wk = generate_sequence(begin, end, n_queries/reps, half_side);

        for(size_t q = 0; q < wk.query_count(); ++q){
            auto& query = wk.queries[q];
            workload->append(query);
            if(workload->query_count() >= n_queries)
                return workload;
        }
    }

    return workload;
}

Workload PeriodicGenerator::generate_sequence(vector<float> begin, vector<float> end, size_t n, float half_side){
    Workload wk;
    vector<float> steps (begin.size());
    // Calculate the steps for each dimension
    for(size_t d = 0; d < begin.size(); ++d){
       steps[d] = (end[d] - begin[d])/n; 
    }

    wk.append(query_from_point(begin, half_side));
    // Now we generate the queries until reaching n-1
    // we still have to append the end that is why n-1
    while(wk.query_count() < n - 1){
        for(size_t d = 0; d < begin.size(); ++d){
            begin[d] += steps[d];
        }
        wk.append(query_from_point(begin, half_side));
    }

    wk.append(query_from_point(end, half_side));
    return wk;
}

Query PeriodicGenerator::query_from_point(vector<float> point, float sel){
    vector<float> lows(point.size());
    vector<float> highs(point.size());
    vector<size_t> cols(point.size());

    for(size_t i = 0; i < point.size(); ++i){
        lows[i] = point[i] - sel;
        highs[i] = point[i] + sel;
        cols[i] = i;
    }

    return Query(lows, highs, cols);
}
