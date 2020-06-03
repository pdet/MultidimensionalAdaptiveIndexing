#ifndef PERIODIC_GENERATOR_H
#define PERIODIC_GENERATOR_H

#include "abstract_generator.hpp"

class PeriodicGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        float selectivity;
        size_t n_queries;
        Workload generate_sequence(vector<float> begin, vector<float> end, size_t n, float half_side);
        Query query_from_point(vector<float> point, float sel);
    public:
        PeriodicGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_
        );
        unique_ptr<Table> generate_table() override;
        unique_ptr<Workload> generate_workload() override;
};

#endif // PERIODIC_GENERATOR_H
