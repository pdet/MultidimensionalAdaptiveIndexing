#ifndef TESTER_H 
#define TESTER_H

#include "index_factory.hpp"
#include "workload.hpp"
#include "table.hpp"
#include <string>
#include <cstdlib>
#include <dirent.h> // to list directories
#include <algorithm> // to sort

class Tester{
    public:
        static void test(size_t algorithm_id, std::map<std::string, std::string>* config = nullptr){
            const string data_folder = "../test/test_data/data";
            const string query_folder = "../test/test_data/queries";
            auto data_files = list_files(data_folder);
            auto query_files = list_files(query_folder);
            for(size_t i = 0; i < data_files.size(); ++i){
                test_algorithm(
                    "../test/test_data/data/"+data_files.at(i),
                    "../test/test_data/queries/"+query_files.at(i),
                    algorithm_id, config
                );
            }
        }

        inline static vector<string> list_files(string folder){
            vector<string> files;
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir (folder.c_str())) != NULL) {
                /* print all the files and directories within directory */
                while ((ent = readdir (dir)) != NULL) {
                    if(string(ent->d_name) == "." || string(ent->d_name) == "..")
                        continue;
                    files.push_back(ent->d_name);
                }
                closedir (dir);
            } else {
                INFO("Couldn't open directory " + folder);
            }
            sort(files.begin(), files.end());
            return files;
        }

        inline static void test_algorithm(string table_path, string workload_path, size_t algorithm_id, std::map<std::string, std::string>* config){
            auto table = Table::read_file(table_path);
            auto workload = Workload::read_file(workload_path);

            INFO("Table info:");
            INFO("Data file: " + table_path);
            INFO("Row count: " + to_string(table->row_count()));
            INFO("Col count: " + to_string(table->col_count()));

            INFO("Workload info");
            INFO("Query file: " + workload_path);
            INFO("Number of queries: " + to_string(workload.query_count()));

            auto alg = config? IndexFactory::getIndex(algorithm_id,*config):  IndexFactory::getIndex(algorithm_id);

            auto baseline = IndexFactory::baselineIndex();

            INFO("Baseline (" + baseline->name() + ")");

            vector<unique_ptr<Table>> baseline_results;
            REQUIRE(workload.query_count() > 0);

            baseline->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
                baseline->adapt_index(workload.queries.at(j));
                auto result = baseline->range_query(workload.queries.at(j));
                baseline_results.push_back(std::move(result));
                //REQUIRE(baseline_results.at(j)->columns[1]->data[0] > 0);
            }

            INFO("Running (" + alg->name() + ")");

            //std::system(("mkdir -p ./'" + alg->name() + "'").c_str());

            alg->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
        //        fprintf(stderr, "%zu \n", j);
                REQUIRE(alg->sanity_check());
                alg->adapt_index(workload.queries.at(j));
                //alg->draw_index("./" + alg->name() + "/" + std::to_string(j) + ".dot");
                auto result = alg->range_query(workload.queries.at(j));
                auto expected = baseline_results.at(j).get();

                INFO("QUERY: " + to_string(j));
                // Check to see if the same amount of tuples was scanned
                auto expected_tuples_scanned = expected->columns[1]->data[0];
                auto result_tuples_scanned = result->columns[1]->data[0];
                REQUIRE(expected_tuples_scanned == result_tuples_scanned);

                // Check if the sum is the same
                auto expected_sum = expected->columns[0]->data[0];
                auto result_sum = result->columns[0]->data[0];
                REQUIRE(expected_sum == result_sum);
            }

        }

        inline static bool file_exists (const std::string& name) {
            if (FILE *file = fopen(name.c_str(), "r")) {
                fclose(file);
                return true;
            } else {
                return false;
            }
        }
};

#endif // TESTER_H
