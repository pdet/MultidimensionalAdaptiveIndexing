#include <catch.hpp>
#include "index_factory.hpp"
#include "data_reader.hpp"
#include "my_generator.hpp"

TEST_CASE( "Check if all algorithms have the same results", "[SmallTest] [FullScan] [CrackingKDTreeBroad] [MedianKDTree] [AverageKDTree] [Quasii]" ) {
    const string workload_path = "small_test_queries";
    const string table_path = "small_test_data";

    auto generator = MyGenerator(
            10000,  // n_of_rows
            5,      // dimensions
            0.5,    // selectivity
            10000   // number_of_queries
            );

    generator.generate(table_path, workload_path);

    auto table = DataReader::read_table(table_path);
    auto workload = DataReader::read_workload(workload_path);

    auto algorithms = IndexFactory::allIndexes();

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

    vector<vector<size_t> > result_sizes(algorithms.size());

    INFO("Executing other indexes");

    for(size_t i = 0; i < algorithms.size(); ++i){
        auto algorithm = algorithms.at(i);
        INFO(algorithm->name());
        algorithm->initialize(table.get());
        for(size_t j = 0; j < workload.size(); ++j){
            algorithm->adapt_index(workload.at(j));
            auto result = algorithm->range_query(workload.at(j));
            result_sizes.at(i).push_back(result.row_count());
        }
    }

    for(size_t query_index = 0; query_index < workload.size(); ++query_index){
        for(size_t alg_index = 0; alg_index < algorithms.size(); ++alg_index){
            INFO("Query: " << query_index << " Algorithm: " << algorithms.at(alg_index)->name());
            REQUIRE(result_sizes.at(alg_index).at(query_index) == baseline_results.at(query_index));
        }
    }
}

