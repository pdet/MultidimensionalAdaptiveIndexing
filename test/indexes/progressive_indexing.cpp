#include <catch.hpp>
#include "tester.hpp"
#include "progressive_index.hpp"


TEST_CASE("Test Progressive Indexing Workload Agnostic","[PIWA]" )
{
    Tester::test(ProgressiveIndex::ID);
}

TEST_CASE("Test Progressive Indexing Workload Dependent","[PIWD]" )
{
    std::map<std::string, std::string> config;
    config.insert(make_pair("workload_adaptive","1"));
    Tester::test(ProgressiveIndex::ID,&config);
}
TEST_CASE("Test Adaptive Progressive Indexing Workload Agnostic","[APIWA]" )
{
    std::map<std::string, std::string> config;
    config.insert(make_pair("interactivity_threshold","1"));
    Tester::test(ProgressiveIndex::ID,&config);
}