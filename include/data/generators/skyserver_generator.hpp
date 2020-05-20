#pragma once
#include "abstract_generator.hpp"

#include <string>
#include <cstdint>

class SkyserverGenerator : public AbstractGenerator{
    public:
        SkyserverGenerator( const std::string &skyserver_dataset_file_,
                const std::string &skyserver_queries_file_
                );
        void generate(std::string table_path, std::string workload_path) override;

    private:
        size_t n_of_rows = 0;
        size_t number_of_queries = 0;
        std::string SKYSERVER_DATASET_FILE = "";
        std::string SKYSERVER_QUERIES_FILE = "";
};
