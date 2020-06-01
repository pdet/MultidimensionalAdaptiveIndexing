#pragma once

#include "abstract_generator.hpp"

class MixedWorkloadGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        float selectivity;
        size_t n_queries;

    std::vector<unique_ptr<Workload>> generators();
    Query get_query(size_t gen_idx, vector<size_t>& generators_indexes, vector<unique_ptr<Workload>> &generators);
    public:
        MixedWorkloadGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_
        );

        unique_ptr<Table> generate_table() override;
        unique_ptr<Workload> generate_workload() override;
};
