#ifndef POWER_GENERATOR_H
#define POWER_GENERATOR_H

#include "abstract_generator.hpp"

#include <string>
#include <cstdint>

class PowerGenerator : public AbstractGenerator{
    public:
        PowerGenerator(
                size_t n_of_rows_,
                size_t number_of_queries_,
                const std::string &power_dataset_file_
                );

        unique_ptr<Table> generate_table() override;
        unique_ptr<Workload> generate_workload() override;
    private:
        size_t n_of_rows = 0;
        size_t number_of_queries = 0;
        size_t dimensions = 3;
        std::vector< std::vector<float> > data_points;
        std::string POWER_DATASET_FILE = "";
};
#endif // POWER_GENERATOR_H
