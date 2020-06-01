#ifndef UNIFORM_GENERATOR_H
#define UNIFORM_GENERATOR_H

#include "abstract_generator.hpp"

class UniformGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        float selectivity;
        size_t n_queries;
    public:
        UniformGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_
        );
        unique_ptr<Table> generate_table() override;
        unique_ptr<Workload> generate_workload() override;
};
#endif // UNIFORM_GENERATOR_H
