#include "power_generator.hpp"

#include <cmath>
#include <cstring>

#include <iostream>
#include <fstream>
#include <limits>
#include <random>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "table.hpp"
#include "workload.hpp"

PowerGenerator::PowerGenerator(
        size_t n_of_rows_,
        size_t number_of_queries_,
        const std::string &power_dataset_file_
        )
    : n_of_rows(n_of_rows_),
    number_of_queries(number_of_queries_),
    POWER_DATASET_FILE(power_dataset_file_)
{
    table = make_unique<Table>(3);
    workload = make_unique<Workload>();
}

void PowerGenerator::generate(std::string table_path, std::string workload_path){
    size_t dimensions = 3;
    std::vector< std::vector<float> > data_points(n_of_rows, std::vector<float>(dimensions));

    std::ifstream tuples(POWER_DATASET_FILE);
    std::string line;
    std::string token;

    for(size_t i = 0; std::getline(tuples, line) && i < n_of_rows; ++i) {
        std::vector<float> data_point(3);
        std::vector<std::string> line_tokens;
        std::istringstream iss(line);
        while(std::getline(iss, token, '\t'))
            line_tokens.push_back(token);
        // parse attributes

        for (size_t j = 1; j < dimensions+1; ++j){
            data_point[j-1] = stof(line_tokens[j]);
        }
        table->append(&(data_point[0]));
        data_points[i] = data_point;
    }

    table->save_file(table_path);

    // random insertion order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(data_points.begin(), data_points.end(), g);

    for (size_t i = 0; i < number_of_queries; ++i) {
        std::vector<float> lb_query(dimensions);
        std::vector<float> ub_query(dimensions);
        int first  = rand() % n_of_rows;
        int second = rand() % n_of_rows;
        for (size_t j = 0; j < dimensions; ++j) {
            lb_query[j] = std::min(data_points[first][j], data_points[second][j]);
            ub_query[j] = std::max(data_points[first][j], data_points[second][j]);
        }
        std::vector<size_t> cols(dimensions);
        for(size_t c = 0; c < dimensions; ++c)
            cols[c] = c;
        workload->append(Query(lb_query, ub_query, cols));
    }
    workload->save_file(workload_path);
}
