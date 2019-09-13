#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <cstdint>

class Generator{
    public:
        Generator(
                int64_t n_of_rows_,
                int64_t dimensions_,
                int64_t workload_,
                float selectivity_,
                int64_t number_of_queries_,
                int64_t query_type_,
                const std::string &power_dataset_file_,
                const std::string &feature_vectors_file_,
                const std::string &genes_file_
                );
        bool generate(const std::string &table_path, const std::string &workload_path);

    private:
        int64_t n_of_rows;
        int64_t dimensions;
        int64_t workload;
        float selectivity;
        int64_t number_of_queries;
        int64_t query_type;
        std::string POWER_DATASET_FILE = "DEBS2012-ChallengeData.txt";
        std::string FEATUREVECTORS_FILE= "chr22_feature.vectors";
        std::string GENES_FILE = "genes.txt";
};
#endif // GENERATOR_H
