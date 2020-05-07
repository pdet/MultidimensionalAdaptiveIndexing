#include <catch.hpp>
#include "tester.hpp"
#include "cracking_kd_tree_per_dimension.hpp"

TEST_CASE(
        "Test CrackingKDTreePerDimension", "[CrackingKDTreePerDimension]" )
{
    Tester::test(CrackingKDTreePerDimension::ID);
}
