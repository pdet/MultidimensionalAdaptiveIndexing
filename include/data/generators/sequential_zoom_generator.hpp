#ifndef SEQUENTIAL_ZOOM_GENERATOR_H
#define SEQUENTIAL_ZOOM_GENERATOR_H

#include "abstract_generator.hpp"

class SequentialZoomGenerator : public AbstractGenerator{
    private:
        size_t n_rows = 0;
        size_t n_dimensions = 0;
        float selectivity = 0.0;
        size_t n_queries = 0;
        bool out = false;
   public:
        SequentialZoomGenerator(
            size_t n_rows_, size_t n_dimensions_,
            float selectivity_, size_t n_queries_,
            bool out_
        );
        void generate(std::string table_path, std::string query_path) override;
};

#endif // SEQUENTIAL_ZOOM_GENERATOR_H
