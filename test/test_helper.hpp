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

            vector<size_t> result_sizes(workload.size());


            INFO("Running (" << alg->name() << ")");

            alg->initialize(table.get());
            for(size_t j = 0; j < workload.size(); ++j){
                alg->adapt_index(workload.at(j));
                auto result = alg->range_query(workload.at(j));
                result_sizes.at(j) = result.row_count();
            }

            INFO("Comparing Results");

            for(size_t query_index = 0; query_index < workload.size(); ++query_index){
                auto received = result_sizes.at(query_index);
                auto expected = baseline_results.at(query_index);
                CHECK(expected == received);
            }
        }
};

#endif // TEST_HELPER_H
