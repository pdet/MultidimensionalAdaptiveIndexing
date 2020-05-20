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

{
    table = make_unique<Table>(2);
    n_of_rows = 67493999 +2;
    number_of_queries = 100001;
    workload = make_unique<Workload>();
}

void SkyserverGenerator::generate(std::string table_path, std::string workload_path){
    size_t dimensions = 2;
    std::vector< std::vector<float> > data_points(n_of_rows, std::vector<float>(dimensions));

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

        for (size_t j = 1; j < dimensions+1; ++j){
            data_point[j-1] = stof(line_tokens[j]);
        }
        table->append(&(data_point[0]));
        data_points[i] = data_point;
    }

    table->save_file(table_path);

    std::ifstream queries(SKYSERVER_QUERIES_FILE);
    //! Skip header
    //! Time to get the workload
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
        std::vector<size_t> cols(dimensions);
        for(size_t c = 0; c < dimensions; ++c)
            cols[c] = c;
        workload->append(Query(lb_query, ub_query, cols));
    }
    workload->save_file(workload_path);
}
