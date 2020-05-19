#include <catch.hpp>
#include "uniform_generator.hpp"
#include "table.hpp"
#include "workload.hpp"
#include "index_factory.hpp"

TEST_CASE( "UniformGenerator, 10000 rows, 5 dim, 0.1 sel, 10000 queries",
           "[UniformGenerator1]" ) {
    const string workload_path = "uniform_generator_queries";
    const string table_path = "uniform_generator_data";

    int64_t n_of_rows = 10000;
    int64_t dimensions = 2;
    float selectivity = 0.1;
    int64_t n_of_queries = 100;

    float error = 0.15;

    auto generator = UniformGenerator(
            n_of_rows,
            dimensions,
            selectivity,
            n_of_queries
            );

    generator.generate(table_path, workload_path);

    auto table = Table::read_file(table_path);
    auto workload = Workload::read_file(workload_path);

    auto baseline = IndexFactory::baselineIndex();

    INFO("Baseline (" << baseline->name() << ")");

    int64_t n_of_selected_rows = n_of_rows * selectivity;
    int64_t error_margin = n_of_selected_rows * error;

    baseline->initialize(table.get());
    for(size_t j = 0; j < workload.query_count(); ++j){
        baseline->adapt_index(workload.queries.at(j));
        auto result = baseline->range_query(workload.queries.at(j))->columns[1]->data[0];
        REQUIRE(result == Approx(n_of_selected_rows).margin(error_margin));
    }
}
TEST_CASE( "UniformGenerator, 10000 rows, 2 dim, 0.1 sel, 10000 queries",
           "[UniformGenerator]" ) {
    const string workload_path = "uniform_generator_queries";
    const string table_path = "uniform_generator_data";

    int64_t n_of_rows = 10000;
    int64_t dimensions = 2;
    float selectivity = 0.1;
    int64_t n_of_queries = 100;

    float error = 0.15;

    auto generator = UniformGenerator(
            n_of_rows,
            dimensions,
            selectivity,
            n_of_queries
            );

    generator.generate(table_path, workload_path);

    auto table = Table::read_file(table_path);
    auto workload = Workload::read_file(workload_path);

    auto baseline = IndexFactory::baselineIndex();

    INFO("Baseline (" << baseline->name() << ")");

    int64_t n_of_selected_rows = n_of_rows * selectivity;
    int64_t error_margin = n_of_selected_rows * error;

    baseline->initialize(table.get());
    for(size_t j = 0; j < workload.query_count(); ++j){
        baseline->adapt_index(workload.queries.at(j));
        auto result = baseline->range_query(workload.queries.at(j))->columns[1]->data[0];
        REQUIRE(result == Approx(n_of_selected_rows).margin(error_margin));
    }
}

