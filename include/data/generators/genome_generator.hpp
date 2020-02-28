#ifndef GENOME_GENERATOR_H
#define GENOME_GENERATOR_H

#include "abstract_generator.hpp"

#include <string>
#include <cstdint>

class GenomeGenerator : public AbstractGenerator{
    public:
        GenomeGenerator(
                int64_t n_of_rows_,
                int64_t number_of_queries_,
                int64_t query_type_,
                const std::string &feature_vectors_file_,
                const std::string &genes_file_
                );
        void generate(std::string table_path, std::string workload_path) override;

    private:
        int64_t n_of_rows;
        int64_t number_of_queries;
        int64_t query_type;
        std::string FEATUREVECTORS_FILE= "chr22_feature.vectors";
        std::string GENES_FILE = "genes.txt";
};
#endif // GENOME_GENERATOR_H
