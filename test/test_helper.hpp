#ifndef TEST_HELPER_H 
#define TEST_HELPER_H

#include "index_factory.hpp"
#include "uniform_generator.hpp"
#include <string>
#include <cstdlib>

class TestHelper{
    public:
        static void test(
                int64_t n_rows, int64_t n_dimensions,
                float selectivity, int64_t n_queries,
                size_t algorithm_id
                ){
            const string workload_path = "test_data/test_queries" + std::to_string(n_dimensions);
            const string table_path = "test_data/test_data" + std::to_string(n_dimensions);

            if(!(TestHelper::file_exists(workload_path) && TestHelper::file_exists(table_path))){
                auto generator = UniformGenerator(
                        n_rows,
                        n_dimensions,
                        selectivity,
                        n_queries
                        );

                generator.generate(table_path, workload_path);
            }

            auto table = Table::read_file(table_path);
            auto workload = Workload::read_file(workload_path);

            auto alg = IndexFactory::getIndex(algorithm_id);

            auto baseline = IndexFactory::baselineIndex();

            INFO("Baseline (" + baseline->name() + ")");

            vector<unique_ptr<Table>> baseline_results;
            REQUIRE(workload.query_count() > 0);

            baseline->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
                baseline->adapt_index(table.get(),workload.queries.at(j));
                auto result = baseline->range_query(table.get(),workload.queries.at(j));
                baseline_results.push_back(std::move(result));
                REQUIRE(baseline_results.at(j)->columns[0]->data[0] > 0);
                REQUIRE(baseline_results.at(j)->columns[1]->data[0] > 0);
            }

            INFO("Running (" + alg->name() + ")");

            //std::system(("mkdir -p ./'" + alg->name() + "'").c_str());

            alg->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
                CHECK(alg->sanity_check());
                alg->adapt_index(table.get(),workload.queries.at(j));
                //alg->draw_index("./" + alg->name() + "/" + std::to_string(j) + ".dot");
                auto result = alg->range_query(table.get(),workload.queries.at(j));
                auto expected = baseline_results.at(j).get();

                // Check to see if the same amount of tuples was scanned
                auto expected_tuples_scanned = expected->columns[1]->data[0];
                auto result_tuples_scanned = result->columns[1]->data[0];
                CHECK(expected_tuples_scanned == result_tuples_scanned);

                // Check if the sum is the same
                auto expected_sum = expected->columns[0]->data[0];
                auto result_sum = result->columns[0]->data[0];
                CHECK(expected_sum == result_sum);
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

#endif // TEST_HELPER_H
