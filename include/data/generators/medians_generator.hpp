#pragma once
#include "abstract_generator.hpp"
#include "kd_tree.hpp"

class MediansGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        size_t partition_size;
        std::unique_ptr<Table> table_copy;

        KDTree* generate_median_kd_tree();
    public:
        MediansGenerator(
            size_t n_rows_, size_t n_dimensions_, size_t partition_size_
        );
        unique_ptr<Table> generate_table() override;
        unique_ptr<Workload> generate_workload() override;
};