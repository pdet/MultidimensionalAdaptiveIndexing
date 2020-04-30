#include <catch.hpp>
#include "table.hpp"
#include <algorithm>
#include <iterator>

TEST_CASE("Test table save and read", "[Table]")
{
    auto n_cols = 3;
    auto n_rows = 2;
    Table table(n_cols, n_rows);
    float row1[] = {1.1, 1.2, 1.3};
    table.append(row1);
    float row2[] = {2.1, 2.2, 2.3};
    table.append(row2);

    table.save_file("table_test_data");

    auto read_table = Table::read_file("table_test_data");
    REQUIRE(read_table->col_count() == n_cols);
    REQUIRE(read_table->row_count() == n_rows);
    REQUIRE(std::equal(std::begin(row1), std::end(row1), read_table->materialize_row(0).get()));
    REQUIRE(std::equal(std::begin(row2), std::end(row2), read_table->materialize_row(1).get()));
}
