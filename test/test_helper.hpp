#ifndef TEST_HELPER_H 
#define TEST_HELPER_H

#include "index_factory.hpp"
#include "data_reader.hpp"
#include "my_generator.hpp"
#include <string>

class TestHelper{
    public:
        static void test(
                int64_t n_rows, int64_t n_dimensions,
                float selectivity, int64_t n_queries,
                std::string algorithm
                ){
            const string workload_path = "test_queries";
            const string table_path = "test_data";

            auto generator = MyGenerator(
                    n_rows,
                    n_dimensions,
                    selectivity,
                    n_queries
                    );

            generator.generate(table_path, workload_path);

            auto table = DataReader::read_table(table_path);
            auto workload = DataReader::read_workload(workload_path);

            auto alg = IndexFactory::getIndex(algorithm);

            auto baseline = IndexFactory::baselineIndex();

            INFO("Baseline (" << baseline->name() << ")");

            vector<size_t> baseline_results(workload.size());

            baseline->initialize(table.get());
            for(size_t j = 0; j < workload.size(); ++j){
                baseline->adapt_index(workload.at(j));
                auto result = baseline->range_query(workload.at(j));
                baseline_results.at(j) = result.row_count();
                REQUIRE(baseline_results.at(j) > 0);
            }

            INFO("Running (" << alg->name() << ")");

            alg->initialize(table.get());
            for(size_t j = 0; j < workload.size(); ++j){
                alg->adapt_index(workload.at(j));
                alg->draw_index("./" + alg->name() + "/" + std::to_string(j) + ".dot");
                auto result = alg->range_query(workload.at(j)).row_count();
                auto expected = baseline_results.at(j);
                CHECK(expected == result);

            }

        }
};

#endif // TEST_HELPER_H
