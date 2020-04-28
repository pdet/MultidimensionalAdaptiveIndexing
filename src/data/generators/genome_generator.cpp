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
        int64_t n_of_rows_,
        int64_t number_of_queries_,
        int64_t query_type_,
        const std::string &feature_vectors_file_,
        const std::string &genes_file_
        )
    :               n_of_rows(n_of_rows_),
    number_of_queries(number_of_queries_),
    query_type(query_type_),
    FEATUREVECTORS_FILE(feature_vectors_file_),
    GENES_FILE(genes_file_)
{
    table = make_unique<Table>(19);
    workload = make_unique<Workload>();
}

void GenomeGenerator::generate(std::string table_path, std::string workload_path){
    auto dimensions = 19;
    std::vector< std::vector<float> > data_points(n_of_rows, std::vector<float>(dimensions));

    int64_t i = 0;
    std::ifstream feature_vectors(FEATUREVECTORS_FILE);
    std::string line;
    std::string token;

    while (std::getline(feature_vectors, line) && i < n_of_rows) {
        std::vector<float> data_point(dimensions);
        std::vector<std::string> line_tokens;
        std::istringstream iss(line);
        while(std::getline(iss, token, ' '))
            line_tokens.push_back(token);
        // parse dimensions

        for (int64_t j = 0; j < dimensions; ++j){
            data_point[j] = stof(line_tokens[j]);
        }
        table->append(&(data_point[0]));
        data_points[i] = data_point;

        i++;
    }

    table->save_file(table_path);

    // random insertion order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(data_points.begin(), data_points.end(), g);

    // Generate queries
    std::vector<std::vector<float> > lb_queries(number_of_queries, std::vector<float>(dimensions, std::numeric_limits<float>::min()));
    std::vector<std::vector<float> > ub_queries(number_of_queries, std::vector<float>(dimensions, std::numeric_limits<float>::max()));
    i = 0;
    std::ifstream genes(GENES_FILE);

    std::ofstream myfile(workload_path);

    std::vector<float> cols;
    while (std::getline(genes, line) && i < number_of_queries) {
        cols.resize(0);
        std::vector<std::string> line_tokens;
        std::istringstream iss(line);
        while(std::getline(iss, token, '\t'))
            line_tokens.push_back(token);
        // Query 2 (chromosome and position)
        lb_queries[i][5] = (float) 21;
        ub_queries[i][5] = (float) 22;
        lb_queries[i][6] = (float) std::stof(line_tokens[4]) - 100000.0;
        ub_queries[i][6] = (float) std::stof(line_tokens[5]) + 200000.0;
        int query_type_;
        if(query_type > 7)
            query_type_ = rand() % 8;
        else
            query_type_ = query_type;
        int rand_point = rand() % n_of_rows;

        // myfile << "SELECT * FROM variations WHERE";
        // myfile << " chromosome = '" << lb_queries[i][5] << "'";
        // myfile << " AND location BETWEEN  '" << lb_queries[i][6] << "' AND '" << ub_queries[i][6] << "'";

        switch (query_type_) {
            // Query 2
            case 1:
                // qual (create range around a certain qual found in the data set)
                lb_queries[i][8] = data_points[rand_point][8] * 0.5;
                ub_queries[i][8] = lb_queries[i][8] * 3;
                // myfile << " AND quality BETWEEN  '" << lb_queries[i][8] << "' AND '" << ub_queries[i][8] << "'";
                // depth (create range around a certain depth found in the data set)
                lb_queries[i][9] = data_points[rand_point][9] * 0.5;
                ub_queries[i][9] = lb_queries[i][9] * 3;
                // myfile << " AND depth BETWEEN  '" << lb_queries[i][9] << "' AND '" << ub_queries[i][9] << "'";
                // allele freq (create range using a certain allele_freq found in the data set)
                lb_queries[i][10] = data_points[rand_point][10];
                ub_queries[i][10] = lb_queries[i][10] + 0.3;
                // myfile << " AND allele_freq BETWEEN  '" << lb_queries[i][10] << "' AND '" << ub_queries[i][10] << "'";
                cols.push_back(5);
                cols.push_back(6);
                cols.push_back(8);
                cols.push_back(9);
                cols.push_back(10);
                break;
                // Query 3
            case 2:
                // gender
                lb_queries[i][2] = data_points[rand_point][2];
                ub_queries[i][2] = lb_queries[i][2];
                // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
                cols.push_back(5);
                cols.push_back(6);
                cols.push_back(2);
                break;
                // Query 4
            case 3:
                // gender
                lb_queries[i][2] = data_points[rand_point][2];
                ub_queries[i][2] = lb_queries[i][2];
                // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
                // population
                lb_queries[i][1] = data_points[rand_point][1];
                ub_queries[i][1] = lb_queries[i][1];
                // myfile << " AND population =  '" << lb_queries[i][1] << "'";
                cols.push_back(5);
                cols.push_back(6);
                cols.push_back(2);
                cols.push_back(1);
                break;
                // Query 5
            case 4:
                // gender
                lb_queries[i][2] = data_points[rand_point][2];
                ub_queries[i][2] = lb_queries[i][2];
                // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
                // population
                lb_queries[i][1] = data_points[rand_point][1];
                ub_queries[i][1] = lb_queries[i][1];
                // myfile << " AND population =  '" << lb_queries[i][1] << "'";
                // relationship
                lb_queries[i][4] = data_points[rand_point][4];
                ub_queries[i][4] = lb_queries[i][4];
                // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
                cols.push_back(5);
                cols.push_back(6);
                cols.push_back(2);
                cols.push_back(1);
                cols.push_back(4);
                break;
                // Query 6
            case 5:
                // gender
                lb_queries[i][2] = data_points[rand_point][2];
                ub_queries[i][2] = lb_queries[i][2];
                // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
                // population
                lb_queries[i][1] = data_points[rand_point][1];
                ub_queries[i][1] = lb_queries[i][1];
                // myfile << " AND population =  '" << lb_queries[i][1] << "'";
                // relationship
                lb_queries[i][4] = data_points[rand_point][4];
                ub_queries[i][4] = lb_queries[i][4];
                // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
                // family_id (create range using a certain family_id found in the data set)
                lb_queries[i][3] = data_points[rand_point][3] * 0.5;
                ub_queries[i][3] = lb_queries[i][3] * 3;
                // myfile << " AND family_id BETWEEN  '" << lb_queries[i][3] << "' AND '" << ub_queries[i][3] << "'";
                cols.push_back(5);
                cols.push_back(6);
                cols.push_back(2);
                cols.push_back(1);
                cols.push_back(4);
                cols.push_back(3);
                break;
                // Query 7
            case 6:
                // gender
                lb_queries[i][2] = data_points[rand_point][2];
                ub_queries[i][2] = lb_queries[i][2];
                // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
                // population
                lb_queries[i][1] = data_points[rand_point][1];
                ub_queries[i][1] = lb_queries[i][1];
                // myfile << " AND population =  '" << lb_queries[i][1] << "'";
                // relationship
                lb_queries[i][4] = data_points[rand_point][4];
                ub_queries[i][4] = lb_queries[i][4];
                // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
                // family_id (create range using a certain family_id found in the data set)
                lb_queries[i][3] = data_points[rand_point][3] * 0.5;
                ub_queries[i][3] = lb_queries[i][3] * 3;
                // myfile << " AND family_id BETWEEN  '" << lb_queries[i][3] << "' AND '" << ub_queries[i][3] << "'";
                // mutation_id (create range using a certain mutation_id found in the data set)
                lb_queries[i][7] = data_points[rand_point][7] * 0.5;
                ub_queries[i][7] = lb_queries[i][7] * 3;
                // myfile << " AND variation_id BETWEEN  '" << lb_queries[i][7] << "' AND '" << ub_queries[i][7] << "'";
                cols.push_back(5);
                cols.push_back(6);
                cols.push_back(2);
                cols.push_back(1);
                cols.push_back(4);
                cols.push_back(3);
                cols.push_back(7);
                break;
                // Query 8
            case 7:
                // qual (create range around a certain qual found in the data set)
                lb_queries[i][8] = data_points[rand_point][8] * 0.5;
                ub_queries[i][8] = lb_queries[i][8] * 3;
                // myfile << " AND quality BETWEEN  '" << lb_queries[i][8] << "' AND '" << ub_queries[i][8] << "'";
                // depth (create range around a certain depth found in the data set)
                lb_queries[i][9] = data_points[rand_point][9] * 0.5;
                ub_queries[i][9] = lb_queries[i][9] * 3;
                // myfile << " AND depth BETWEEN  '" << lb_queries[i][9] << "' AND '" << ub_queries[i][9] << "'";
                // allele freq (create range using a certain allele_freq found in the data set)
                lb_queries[i][10] = data_points[rand_point][10];
                ub_queries[i][10] = lb_queries[i][10] + 0.3;
                // myfile << " AND allele_freq BETWEEN  '" << lb_queries[i][10] << "' AND '" << ub_queries[i][10] << "'";
                // ref_base
                lb_queries[i][13] = data_points[rand_point][13];
                ub_queries[i][13] = lb_queries[i][13];
                // myfile << " AND ref_base =  '" << lb_queries[i][13] << "'";
                // alt_base
                lb_queries[i][14] = data_points[rand_point][14];
                ub_queries[i][14] = lb_queries[i][14];
                // myfile << " AND alt_base =  '" << lb_queries[i][14] << "'";
                // ancestral_allele
                lb_queries[i][16] = data_points[rand_point][16];
                ub_queries[i][16] = lb_queries[i][16];
                // myfile << " AND ancestral_allele =  '" << lb_queries[i][16] << "'";
                // mutation_id (create range using a certain mutation_id found in the data set)
                lb_queries[i][7] = data_points[rand_point][7] * 0.5;
                ub_queries[i][7] = lb_queries[i][7] * 3;
                // myfile << " AND variation_id BETWEEN  '" << lb_queries[i][7] << "' AND '" << ub_queries[i][7] << "'";
                // sample_id
                lb_queries[i][0] = data_points[rand_point][0];
                ub_queries[i][0] = lb_queries[i][0];
                // myfile << " AND sample_id BETWEEN  '" << lb_queries[i][0] << "' AND '" << ub_queries[i][0] << "'";
                // gender
                lb_queries[i][2] = data_points[rand_point][2];
                ub_queries[i][2] = lb_queries[i][2];
                // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
                // family_id (create range using a certain family_id found in the data set)
                lb_queries[i][3] = data_points[rand_point][3] * 0.5;
                ub_queries[i][3] = lb_queries[i][3] * 3;
                // myfile << " AND family_id BETWEEN  '" << lb_queries[i][3] << "' AND '" << ub_queries[i][3] << "'";
                // population
                lb_queries[i][1] = data_points[rand_point][1];
                ub_queries[i][1] = lb_queries[i][1];
                // myfile << " AND population =  '" << lb_queries[i][1] << "'";
                // relationship
                lb_queries[i][4] = data_points[rand_point][4];
                ub_queries[i][4] = lb_queries[i][4];
                // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
                // variant_type
                lb_queries[i][15] = data_points[rand_point][15];
                ub_queries[i][15] = lb_queries[i][15];
                // myfile << " AND variant_type =  '" << lb_queries[i][15] << "'";
                // genotypegenotype
                lb_queries[i][17] = data_points[rand_point][17];
                ub_queries[i][17] = lb_queries[i][17];
                // myfile << " AND genotype =  '" << lb_queries[i][17] << "'";
                // allele_count (create range using a certain allele_count found in the data set)
                lb_queries[i][11] = data_points[rand_point][11] * 0.5;
                ub_queries[i][11] = lb_queries[i][11] * 3;
                // myfile << " BUG";
                // filter
                lb_queries[i][12] = data_points[rand_point][12];
                ub_queries[i][12] = lb_queries[i][12];
                // myfile << " BUG";
                // reference genome
                lb_queries[i][18] = data_points[rand_point][18];
                ub_queries[i][18] = lb_queries[i][18];
                // myfile << " BUG";
                cols.push_back(5);
                cols.push_back(6);
                cols.push_back(8);
                cols.push_back(9);
                cols.push_back(10);
                cols.push_back(13);
                cols.push_back(14);
                cols.push_back(16);
                cols.push_back(7);
                cols.push_back(0);
                cols.push_back(2);
                cols.push_back(3);
                cols.push_back(1);
                cols.push_back(4);
                cols.push_back(15);
                cols.push_back(17);
                cols.push_back(11);
                cols.push_back(12);
                cols.push_back(18);
                break;
                // Query 1
            case 0:
                cols.push_back(5);
                cols.push_back(6);
            default:
                break;
        }
        for(size_t col = 0; col < cols.size(); ++col){
            myfile << lb_queries[i][cols[col]];
            if(col + 1 != cols.size())
                myfile << " ";
        }
        myfile << "\n";

        for(size_t col = 0; col < cols.size(); ++col){
            myfile << ub_queries[i][cols[col]];
            if(col + 1 != cols.size())
                myfile << " ";
        }
        myfile << "\n";

        for(size_t col = 0; col < cols.size(); ++col){
            myfile << cols[col];
            if(col + 1 != cols.size())
                myfile << " ";
        }
        myfile << "\n";
        i++;
    }
    myfile.close();
}
