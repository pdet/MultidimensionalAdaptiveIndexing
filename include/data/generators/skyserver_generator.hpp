#pragma once
#include "abstract_generator.hpp"

#include <string>
#include <cstdint>

class SkyserverGenerator : public AbstractGenerator{
    public:
        SkyserverGenerator( const std::string &skyserver_dataset_file_,
                const std::string &skyserver_queries_file_
                );

        unique_ptr<Table> generate_table() override;
        unique_ptr<Workload> generate_workload() override;

    private:
        size_t n_of_rows = 67493999 +2;
        size_t number_of_queries = 100001;
        size_t n_dimensions = 2;
        std::string SKYSERVER_DATASET_FILE = "";
        std::string SKYSERVER_QUERIES_FILE = "";
};
