#ifndef IMBALANCED_COLUMN_GENERATOR_H
#define IMBALANCED_COLUMN_GENERATOR_H

#include "abstract_generator.hpp"

class ImbalancedColumnGenerator : public AbstractGenerator{
    private:
        size_t n_rows;
        size_t n_dimensions;
        size_t n_queries;
        bool ascending; // If true then the first column selects the fewest, if false than the last column selects the fewest
    public:
        ImbalancedColumnGenerator(
            size_t n_rows_, size_t n_queries_, bool ascending_
        );
        unique_ptr<Table> generate_table() override;
        unique_ptr<Workload> generate_workload() override;
};
#endif // IMBALANCED_COLUMN_GENERATOR_H
