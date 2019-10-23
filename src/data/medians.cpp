#include "medians.hpp"
#include <fstream>
#include <random>
#include "data_reader.hpp"
#include "median_kd_tree.hpp"
#include <map>
#include <string>

Medians::Medians(
        int64_t n_of_rows_,
        int64_t dimensions_
): n_of_rows(n_of_rows_), dimensions(dimensions_){}
void Medians::generate(
    const std::string &table_path,
    const std::string &workload_path
){
    // Generate Data
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(0, n_of_rows);

    std::ofstream table_file(table_path);

    for(int64_t i = 0; i < n_of_rows; ++i){
        for(int64_t j = 0; j < dimensions; ++j){
           table_file << distr(generator); 
           if(j + 1 != dimensions)
               table_file << " ";

        }
        table_file << std::endl;
    }

    table_file.close();

    // Generator Queries
    std::ofstream query_file(workload_path);
    auto table = DataReader::read_table(table_path);
    auto index = make_unique<MedianKDTree>(std::map<std::string, std::string>());
    index->initialize(table.get());

    auto current = index->index->root.get();
    
    while(current != nullptr){
        query_file << current->key << '\n';
        query_file << current->key << '\n';
        query_file << current->column << '\n';
        current = current->left_child.get();
    }

    query_file.close();
}
