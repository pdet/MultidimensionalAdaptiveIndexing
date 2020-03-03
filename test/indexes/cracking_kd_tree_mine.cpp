#include <catch.hpp>
#include "test_helper.hpp"
#include "cracking_kd_tree_mine.hpp"

//TEST_CASE(
//        "Test Cracking KDTree\
//        [1000000 rows, 2 dimensions, 0.001 selectivity, 1000 queries]",
//        "[CrackingKDTree]" )
//{
//    TestHelper::test(1000000, 2, 0.001, 1000, CrackingKDTree::ID);
//}
//
//TEST_CASE(
//        "Test Cracking KDTree\
//        [1000000 rows, 16 dimensions, 0.001 selectivity, 1000 queries]",
//        "[CrackingKDTree]" )
//{
//    TestHelper::test(1000000, 16, 0.001, 1000, CrackingKDTree::ID);
//}
TEST_CASE(
        "Test Cracking KDTree Mine\
        [10000 rows, 5 dimensions, 0.5 selectivity, 1000 queries]",
        "[CrackingKDTreeMine]" )
{
    TestHelper::test(10000, 5, 0.5, 1000, CrackingKDTreeMine::ID);
}

TEST_CASE(
        "Test Cracking KDTree Mine\
        [100000 rows, 2 dimensions, 0.1 selectivity, 1000 queries]",
        "[CrackingKDTreeMine]" )
{
    TestHelper::test(100000, 2, 0.1, 1000, CrackingKDTreeMine::ID);
}
