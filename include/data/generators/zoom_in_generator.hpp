#ifndef ZOOM_IN_GENERATOR_H
#define ZOOM_IN_GENERATOR_H

#include "abstract_generator.hpp"

class ZoomInGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        float selectivity;
        size_t n_queries;
    public:
        ZoomInGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_
        );
        void generate(std::string table_path, std::string query_path) override;
};

#endif // ZOOM_IN_GENERATOR_H
