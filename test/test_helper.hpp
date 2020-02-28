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

            vector<size_t> baseline_results(workload.query_count());

            baseline->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
                baseline->adapt_index(workload.queries.at(j));
                auto result = baseline->range_query(workload.queries.at(j));
                baseline_results.at(j) = result.row_count();
                REQUIRE(baseline_results.at(j) > 0);
            }

            INFO("Running (" << alg->name() << ")");

            //std::system(("mkdir -p ./'" + alg->name() + "'").c_str());

            alg->initialize(table.get());
            for(size_t j = 0; j < workload.query_count(); ++j){
                alg->adapt_index(workload.queries.at(j));
                //alg->draw_index("./" + alg->name() + "/" + std::to_string(j) + ".dot");
                auto result = alg->range_query(workload.queries.at(j)).row_count();
                auto expected = baseline_results.at(j);
                CHECK(expected == result);
            }

        }
};

#endif // TEST_HELPER_H
