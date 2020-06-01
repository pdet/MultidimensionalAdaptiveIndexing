#include "genome_generator.hpp"

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

GenomeGenerator::GenomeGenerator(
        size_t n_of_rows_,
        size_t number_of_queries_,
        size_t query_type_,
        const std::string &feature_vectors_file_,
        const std::string &genes_file_
        )
    :               n_of_rows(n_of_rows_),
    number_of_queries(number_of_queries_),
    query_type(query_type_),
    FEATUREVECTORS_FILE(feature_vectors_file_),
    GENES_FILE(genes_file_)
{}

unique_ptr<Table> GenomeGenerator::generate_table(){
    auto table = make_unique<Table>(dimensions);
    srand (20);

    data_points.resize(n_of_rows, std::vector<float>(dimensions));
    std::ifstream feature_vectors(FEATUREVECTORS_FILE);
    std::string line;
    std::string token;

    for(size_t i = 0; std::getline(feature_vectors, line) && i < n_of_rows; ++i) {
        std::vector<float> data_point(dimensions);
        std::vector<std::string> line_tokens;
        std::istringstream iss(line);
        while(std::getline(iss, token, ' '))
            line_tokens.push_back(token);
        // parse dimensions

        for (size_t j = 0; j < dimensions; ++j){
            data_point[j] = stof(line_tokens[j]);
        }
        table->append(&(data_point[0]));
        data_points[i] = data_point;
    }
    return table;
}

unique_ptr<Workload> GenomeGenerator::generate_workload(){
    // Generate queries
    if(data_points.size() == 0){
        // If table wasnt generated, then we generate it here
        generate_table();
    }
    auto workload = make_unique<Workload>();
    std::ifstream genes(GENES_FILE);
    std::string line;
    std::string token;
    for(size_t i = 0; std::getline(genes, line) && i < number_of_queries; ++i) {
        std::vector<float> lb_query(dimensions, std::numeric_limits<float>::min());
        std::vector<float> ub_query(dimensions, std::numeric_limits<float>::max());

        std::vector<std::string> line_tokens;
        std::istringstream iss(line);
        while(std::getline(iss, token, '\t'))
            line_tokens.push_back(token);
        // Query 2 (chromosome and position)
        lb_query[5] = (float) 22;
        ub_query[5] = (float) 22;
        lb_query[6] = (float) std::stof(line_tokens[4]) - 100000.0;
        ub_query[6] = (float) std::stof(line_tokens[5]) + 200000.0;

        int query_type_;
        if(query_type > 7)
            query_type_ = rand() % 8;
        else
            query_type_ = query_type;
        int rand_point = rand() % n_of_rows;

        switch (query_type_) {
            case 0:
                // Query 1
                break;
            // Query 2
            case 1:
                // qual (create range around a certain qual found in the data set)
                lb_query[8] = data_points[rand_point][8] * 0.5;
                ub_query[8] = lb_query[8] * 3;
                // depth (create range around a certain depth found in the data set)
                lb_query[9] = data_points[rand_point][9] * 0.5;
                ub_query[9] = lb_query[9] * 3;
                // allele freq (create range using a certain allele_freq found in the data set)
                lb_query[10] = data_points[rand_point][10];
                ub_query[10] = lb_query[10] + 0.3;
                break;
                // Query 3
            case 2:
                // gender
                lb_query[2] = data_points[rand_point][2];
                ub_query[2] = lb_query[2];
                break;
                // Query 4
            case 3:
                // gender
                lb_query[2] = data_points[rand_point][2];
                ub_query[2] = lb_query[2];
                // population
                lb_query[1] = data_points[rand_point][1];
                ub_query[1] = lb_query[1];
                break;
                // Query 5
            case 4:
                // gender
                lb_query[2] = data_points[rand_point][2];
                ub_query[2] = lb_query[2];
                // population
                lb_query[1] = data_points[rand_point][1];
                ub_query[1] = lb_query[1];
                // relationship
                lb_query[4] = data_points[rand_point][4];
                ub_query[4] = lb_query[4];
                break;
                // Query 6
            case 5:
                // gender
                lb_query[2] = data_points[rand_point][2];
                ub_query[2] = lb_query[2];
                // population
                lb_query[1] = data_points[rand_point][1];
                ub_query[1] = lb_query[1];
                // relationship
                lb_query[4] = data_points[rand_point][4];
                ub_query[4] = lb_query[4];
                // family_id (create range using a certain family_id found in the data set)
                lb_query[3] = data_points[rand_point][3] * 0.5;
                ub_query[3] = lb_query[3] * 3;
                break;
                // Query 7
            case 6:
                // gender
                lb_query[2] = data_points[rand_point][2];
                ub_query[2] = lb_query[2];
                // population
                lb_query[1] = data_points[rand_point][1];
                ub_query[1] = lb_query[1];
                // relationship
                lb_query[4] = data_points[rand_point][4];
                ub_query[4] = lb_query[4];
                // family_id (create range using a certain family_id found in the data set)
                lb_query[3] = data_points[rand_point][3] * 0.5;
                ub_query[3] = lb_query[3] * 3;
                // mutation_id (create range using a certain mutation_id found in the data set)
                lb_query[7] = data_points[rand_point][7] * 0.5;
                ub_query[7] = lb_query[7] * 3;
                break;
                // Query 8
            case 7:
                // sample_id
                lb_query[0] = data_points[rand_point][0];
                ub_query[0] = lb_query[0];
                // population
                lb_query[1] = data_points[rand_point][1];
                ub_query[1] = lb_query[1];
                // gender
                lb_query[2] = data_points[rand_point][2];
                ub_query[2] = lb_query[2];
                // family_id (create range using a certain family_id found in the data set)
                lb_query[3] = data_points[rand_point][3] * 0.5;
                ub_query[3] = lb_query[3] * 3;
                // relationship
                lb_query[4] = data_points[rand_point][4];
                ub_query[4] = lb_query[4];
                // mutation_id (create range using a certain mutation_id found in the data set)
                lb_query[7] = data_points[rand_point][7] * 0.5;
                ub_query[7] = lb_query[7] * 3;
                // qual (create range around a certain qual found in the data set)
                lb_query[8] = data_points[rand_point][8] * 0.5;
                ub_query[8] = lb_query[8] * 3;
                // depth (create range around a certain depth found in the data set)
                lb_query[9] = data_points[rand_point][9] * 0.5;
                ub_query[9] = lb_query[9] * 3;
                // allele freq (create range using a certain allele_freq found in the data set)
                lb_query[10] = data_points[rand_point][10];
                ub_query[10] = lb_query[10] + 0.3;
                // allele_count (create range using a certain allele_count found in the data set)
                lb_query[11] = data_points[rand_point][11] * 0.5;
                ub_query[11] = lb_query[11] * 3;
                // filter
                lb_query[12] = data_points[rand_point][12];
                ub_query[12] = lb_query[12];
                // ref_base
                lb_query[13] = data_points[rand_point][13];
                ub_query[13] = lb_query[13];
                // alt_base
                lb_query[14] = data_points[rand_point][14];
                ub_query[14] = lb_query[14];
                // variant_type
                lb_query[15] = data_points[rand_point][15];
                ub_query[15] = lb_query[15];
                // ancestral_allele
                lb_query[16] = data_points[rand_point][16];
                ub_query[16] = lb_query[16];
                // genotypegenotype
                lb_query[17] = data_points[rand_point][17];
                ub_query[17] = lb_query[17];
                // reference genome
                lb_query[18] = data_points[rand_point][18];
                ub_query[18] = lb_query[18];
                break;
            default:
                break;
        }
        std::vector<size_t> cols(dimensions);
        for(size_t c = 0; c < dimensions; ++c)
            cols[c] = c;
        workload->append(Query(lb_query, ub_query, cols));
    }
    return workload;
}
