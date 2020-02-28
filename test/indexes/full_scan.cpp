#include <catch.hpp>
#include "test_helper.hpp"
#include "full_scan.hpp"

TEST_CASE(
        "Test FullScan\
        [10000 rows, 5 dimensions, 0.5 selectivity, 1000 queries]",
        "[FullScan]" )
{
    TestHelper::test(10000, 5, 0.5, 1000, FullScan::ID);
}

TEST_CASE(
        "Test FullScan\
        [100000 rows, 2 dimensions, 0.1 selectivity, 1000 queries]",
        "[FullScan]" )
{
    TestHelper::test(100000, 2, 0.1, 1000, FullScan::ID);
}
