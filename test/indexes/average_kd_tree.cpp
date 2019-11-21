#include <catch.hpp>
#include "test_helper.hpp"

TEST_CASE(
        "Test Average KDTree\
        [10000 rows, 5 dimensions, 0.5 selectivity, 1000 queries]",
        "[AverageKDTree]" )
{
    TestHelper::test(10000, 5, 0.5, 1000, "KDTree-Average");
}

TEST_CASE(
        "Test Average KDTree\
        [100000 rows, 2 dimensions, 0.1 selectivity, 1000 queries]",
        "[AverageKDTree]" )
{
    TestHelper::test(100000, 2, 0.1, 1000, "KDTree-Average");
}
