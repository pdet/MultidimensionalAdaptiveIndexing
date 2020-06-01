#include "skyserver_generator.hpp"

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

SkyserverGenerator::SkyserverGenerator(
        const std::string &skyserver_dataset_file_,
        const std::string &skyserver_queries_file_
        )
    :
    SKYSERVER_DATASET_FILE(skyserver_dataset_file_),
    SKYSERVER_QUERIES_FILE(skyserver_queries_file_)

{}

unique_ptr<Table> SkyserverGenerator::generate_table(){
    auto table = make_unique<Table>(n_dimensions);
    std::vector< std::vector<float> > data_points(n_of_rows, std::vector<float>(n_dimensions));

    std::ifstream tuples(SKYSERVER_DATASET_FILE);
    std::string line;
    std::string token;
    //! Skip header
    std::getline(tuples, line);
    for(size_t i = 0; std::getline(tuples, line) && i < n_of_rows; ++i) {
        std::vector<float> data_point(2);
        std::vector<std::string> line_tokens;
        std::istringstream iss(line);
        while(std::getline(iss, token, ','))
            line_tokens.push_back(token);
        // parse attributes

        for (size_t j = 1; j < n_dimensions+1; ++j){
            data_point[j-1] = stof(line_tokens[j]);
        }
        table->append(&(data_point[0]));
        data_points[i] = data_point;
    }

    return table;
}

unique_ptr<Workload> SkyserverGenerator::generate_workload(){
    std::ifstream queries(SKYSERVER_QUERIES_FILE);
    auto workload = make_unique<Workload>();
    //! Skip header
    //! Time to get the workload
    std::string line;
    std::string token;
    for(size_t i = 0; std::getline(queries, line) && i < number_of_queries; ++i) {
        std::vector<float> lb_query;
        std::vector<float> ub_query;
        std::vector<std::string> line_tokens;
        std::istringstream iss(line);
        while(std::getline(iss, token, ','))
            line_tokens.push_back(token);

        // parse attributes
        lb_query.push_back(stof(line_tokens[0]));
        lb_query.push_back(stof(line_tokens[2]));
        ub_query.push_back(stof(line_tokens[1]));
        ub_query.push_back(stof(line_tokens[3]));
        std::vector<size_t> cols(n_dimensions);
        for(size_t c = 0; c < n_dimensions; ++c)
            cols[c] = c;
        workload->append(Query(lb_query, ub_query, cols));
    }
    return workload;
}
