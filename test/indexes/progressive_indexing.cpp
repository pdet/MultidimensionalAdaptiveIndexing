#include <catch.hpp>
#include "tester.hpp"
#include "progressive_index.hpp"


TEST_CASE("Test Progressive Indexing ","[PI]" )
{
    Tester::test(ProgressiveIndex::ID);
}

TEST_CASE("Test Adaptive Progressive Indexing ","[API]" )
{
    std::map<std::string, std::string> config;
    config.insert(make_pair("interactivity_threshold","1"));
    Tester::test(ProgressiveIndex::ID,&config);
}

//! We continue as we are until we are below the interactivity threshold.
TEST_CASE("Test Adaptive Progressive Indexing Threshold","[API]" )
{
    std::map<std::string, std::string> config;
    config.insert(make_pair("interactivity_threshold","0.2"));
    config.insert(make_pair("interactivity_threshold_is_time","1"));
    Tester::test(ProgressiveIndex::ID,&config);
}

//! At most x queries above the interactivity threshold.
TEST_CASE("Test Adaptive Progressive Indexing Threshold X Queries","[API]" )
{
    std::map<std::string, std::string> config;
    config.insert(make_pair("interactivity_threshold","0.2"));
    config.insert(make_pair("interactivity_threshold_is_time","1"));
    config.insert(make_pair("num_queries_over","10"));
    Tester::test(ProgressiveIndex::ID,&config);
}