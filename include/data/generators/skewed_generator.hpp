#ifndef SKEWED_GENERATOR_H
#define SKEWED_GENERATOR_H

#include "abstract_generator.hpp"

class SkewedGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        float selectivity;
        size_t n_queries;
    public:
        SkewedGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_
        );
        void generate(std::string table_path, std::string query_path) override;
};

#endif // SKEWED_GENERATOR_H
