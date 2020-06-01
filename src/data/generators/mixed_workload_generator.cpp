#include "mixed_workload_generator.hpp"

#include "uniform_generator.hpp"
#include "skewed_generator.hpp"
#include "sequential_generator.hpp"
#include "periodic_generator.hpp"
#include "zoom_generator.hpp"
#include "sequential_zoom_generator.hpp"
#include "alternating_zoom_generator.hpp"

#include <random>
#include <vector>
#include <math.h>


MixedWorkloadGenerator::MixedWorkloadGenerator(
    size_t n_rows_, size_t n_dimensions_,
    float selectivity_, size_t n_queries_
) : n_rows(n_rows_), n_dimensions(n_dimensions_),
    selectivity(selectivity_), n_queries(n_queries_)
{}

unique_ptr<Table> MixedWorkloadGenerator::generate_table(){
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

unique_ptr<Workload> MixedWorkloadGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();

    auto workloads = generators();

    size_t window = n_queries * 0.05 + 1;

    vector<size_t> workloads_indexes(workloads.size(), 0);

    size_t gen_idx = 0;

    for(size_t i = 0; i < n_queries; ++i){
        workload->append(get_query(gen_idx, workloads_indexes, workloads));
        if(i % window == 0){
            gen_idx = (gen_idx + 1) % workloads_indexes.size();
        }
    }

    return workload;
}

Query MixedWorkloadGenerator::get_query(size_t gen_idx, vector<size_t>& generators_indexes, vector<unique_ptr<Workload>> &generators){
    size_t query_idx = generators_indexes[gen_idx];
    generators_indexes[gen_idx] = (generators_indexes[gen_idx] + 1) % generators[gen_idx]->query_count();
    return generators[gen_idx]->queries[query_idx];
}

vector<unique_ptr<Workload>> MixedWorkloadGenerator::generators(){
    vector<unique_ptr<Workload>> generators;
    generators.push_back(
        UniformGenerator(
            n_rows, n_dimensions, selectivity, n_queries
        ).generate_workload()
    );
    generators.push_back(
        SkewedGenerator(
            n_rows, n_dimensions, selectivity, n_queries
        ).generate_workload()
    );
    generators.push_back(
        SequentialGenerator(
            n_rows, n_dimensions, selectivity, n_queries
        ).generate_workload()
    );
    generators.push_back(
        PeriodicGenerator(
            n_rows, n_dimensions, selectivity, n_queries
        ).generate_workload()
    );
    generators.push_back(
        ZoomGenerator(
            n_rows, n_dimensions, selectivity, n_queries, false
        ).generate_workload()
    );
    generators.push_back(
        SequentialZoomGenerator(
            n_rows, n_dimensions, selectivity, n_queries, false
        ).generate_workload()
    );
    generators.push_back(
        AlternatingZoomGenerator(
            n_rows, n_dimensions, selectivity, n_queries, false
        ).generate_workload()
    );
    return generators;
}


// keep collecting the queries from their workloads as necessary
