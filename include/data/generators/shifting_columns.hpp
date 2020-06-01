#pragma once
#include "abstract_generator.hpp"

class ShiftingColumnsGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        float selectivity;
        size_t n_queries;
    public:
        ShiftingColumnsGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_
        );
        void generate(std::string table_path, std::string query_path) override;
};
