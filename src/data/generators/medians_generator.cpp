#include "medians_generator.hpp"
#include <random>
#include <vector>
#include <math.h>
#include <limits>
#include "median_kd_tree.hpp"


MediansGenerator::MediansGenerator(
        size_t n_rows_, size_t n_dimensions_, size_t partition_size_
) : n_rows(n_rows_), n_dimensions(n_dimensions_), partition_size(partition_size_)
{
}

unique_ptr<Table> MediansGenerator::generate_table(){
    // Generate Data
    auto table = make_unique<Table>(n_dimensions);
    std::mt19937 generator(0);
    std::uniform_int_distribution<int> distr(0, n_rows);

    for(size_t i = 0; i < n_rows; ++i){
        float* row = new float[n_dimensions];
        for(size_t j = 0; j < n_dimensions; ++j){
            row[j] = distr(generator);
        }
        table->append(row);
        delete[] row;
    }

    // copy generated table
    table_copy = make_unique<Table>(table.get());

    return table;
}

unique_ptr<Workload> MediansGenerator::generate_workload(){
    // Generator Queries
    auto workload = make_unique<Workload>();

    // Generate a median kd-tree
    auto index = generate_median_kd_tree();

    // Follow the left most path to a leaf and use the keys as value
    auto current = index->root.get();
    while(current != nullptr){
        std::vector<float> lows(n_dimensions);
        std::vector<float> highs(n_dimensions);
        std::vector<size_t> cols(n_dimensions);

        for(size_t j = 0; j < n_dimensions; ++j){
            lows.at(j) = numeric_limits<float>::lowest();
            highs.at(j) = numeric_limits<float>::max();
            cols.at(j) = j;
        }

        lows.at(current->column) = current->key;
        highs.at(current->column) = current->key;

        workload->append(
                Query(lows, highs, cols)
        );

        current = current->left_child.get();
    }
    return workload;
}

KDTree* MediansGenerator::generate_median_kd_tree(){
    auto config = std::map<std::string, std::string>();
    config["minimum_partition_size"] = to_string(partition_size);
    auto median_kd_tree_index = new MedianKDTree(config);
    median_kd_tree_index->initialize(table_copy.get());
    return median_kd_tree_index->get_index();
}
