#include <catch.hpp>
#include "data_reader.hpp"
#include "my_generator.hpp"
#include "index_factory.hpp"

TEST_CASE( "My data generator, 10000 rows, 5 dim, 0.1 sel, 10000 queries",
           "[MyDataGenerator]" ) {
    const string workload_path = "my_generator_queries";
    const string table_path = "my_generator_data";

    int64_t n_of_rows = 10000;
    int64_t dimensions = 5;
    float selectivity = 0.1;
    int64_t n_of_queries = 10000;

    float error = 0.15;

    auto generator = MyGenerator(
            n_of_rows,
            dimensions,
            selectivity,
            n_of_queries
            );

    generator.generate(table_path, workload_path);

    auto table = DataReader::read_table(table_path);
    auto workload = DataReader::read_workload(workload_path);

    auto baseline = IndexFactory::baselineIndex();

    INFO("Baseline (" << baseline->name() << ")");

    int64_t n_of_selected_rows = n_of_rows * selectivity;
    int64_t error_margin = n_of_selected_rows * error;

    baseline->initialize(table.get());
    for(size_t j = 0; j < workload.size(); ++j){
        baseline->adapt_index(workload.at(j));
        auto result = baseline->range_query(workload.at(j)).row_count();
        REQUIRE(result == Approx(n_of_selected_rows).margin(error_margin));
    }
}
TEST_CASE( "My data generator, 10000 rows, 2 dim, 0.1 sel, 10000 queries",
           "[MyDataGenerator]" ) {
    const string workload_path = "my_generator_queries";
    const string table_path = "my_generator_data";

    int64_t n_of_rows = 10000;
    int64_t dimensions = 22;
    float selectivity = 0.1;
    int64_t n_of_queries = 10000;

    float error = 0.15;

    auto generator = MyGenerator(
            n_of_rows,
            dimensions,
            selectivity,
            n_of_queries
            );

    generator.generate(table_path, workload_path);

    auto table = DataReader::read_table(table_path);
    auto workload = DataReader::read_workload(workload_path);

    auto baseline = IndexFactory::baselineIndex();

    INFO("Baseline (" << baseline->name() << ")");

    int64_t n_of_selected_rows = n_of_rows * selectivity;
    int64_t error_margin = n_of_selected_rows * error;

    baseline->initialize(table.get());
    for(size_t j = 0; j < workload.size(); ++j){
        baseline->adapt_index(workload.at(j));
        auto result = baseline->range_query(workload.at(j)).row_count();
        REQUIRE(result == Approx(n_of_selected_rows).margin(error_margin));
    }
}

