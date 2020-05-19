#ifndef SEQUENTIAL_ZOOM_OUT_GENERATOR_H
#define SEQUENTIAL_ZOOM_OUT_GENERATOR_H

#include "abstract_generator.hpp"

class SequentialZoomOutGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        float selectivity;
        size_t n_queries;
    public:
        SequentialZoomOutGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_
        );
        void generate(std::string table_path, std::string query_path) override;
};

#endif // SEQUENTIAL_ZOOM_OUT_GENERATOR_H
