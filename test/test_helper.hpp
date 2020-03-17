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
            const string workload_path = "test_queries";
            const string table_path = "test_data";

            auto generator = UniformGenerator(
                    n_rows,
                    n_dimensions,
                    selectivity,
                    n_queries
                    );

            generator.generate(table_path, workload_path);

            auto table = Table::read_file(table_path);
            auto workload = Workload::read_file(workload_path);

            auto alg = IndexFactory::getIndex(algorithm_id);

            auto baseline = IndexFactory::baselineIndex();

            INFO("Baseline (" << baseline->name() << ")");

            vector<unique_ptr<Table>> baseline_results;
            REQUIRE(workload.query_count() > 0);

            baseline->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
                baseline->adapt_index(workload.queries.at(j));
                auto result = baseline->range_query(workload.queries.at(j));
                baseline_results.push_back(std::move(result));
                REQUIRE(baseline_results.at(j)->row_count() > 0);
            }

            INFO("Running (" << alg->name() << ")");

            //std::system(("mkdir -p ./'" + alg->name() + "'").c_str());

            alg->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
                alg->adapt_index(workload.queries.at(j));
                //alg->draw_index("./" + alg->name() + "/" + std::to_string(j) + ".dot");
                auto result = alg->range_query(workload.queries.at(j));
                auto expected = baseline_results.at(j).get();
                CHECK(expected->row_count() == result->row_count());

                // check if the elements are the same
                auto expected_elements = expected->columns[0]->data;
                auto result_elements = result->columns[0]->data;
                sort(expected_elements, expected_elements + expected->row_count());
                sort(result_elements, result_elements + result->row_count());
                for(auto i = 0; i < result->row_count(); ++i){
                    CHECK(expected_elements[i] == result_elements[i]);
                }
            }

        }
};

#endif // TEST_HELPER_H
