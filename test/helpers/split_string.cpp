#include <catch.hpp>
#include "split_string.hpp"

TEST_CASE( "Test split string with floats", "[SplitString]")
{
    std::string line = "10.0 11.0 12.0  13.0";
    std::vector<float> expected = {10, 11, 12, 13};
    auto result = SplitString<float>::split(line, " ");
    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}

TEST_CASE( "Test split string with integers", "[SplitString]")
{
    std::string line = "10 11 12  13";
    std::vector<int64_t> expected = {10, 11, 12, 13};
    auto result = SplitString<int64_t>::split(line, " ");
    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}

TEST_CASE( "Test split string with floats but integers in the string", "[SplitString]")
{
    std::string line = "10 11 12  13";
    std::vector<float> expected = {10.0, 11.0, 12.0, 13.0};
    auto result = SplitString<float>::split(line, " ");
    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}
TEST_CASE( "Test split string with integers but with floats in the string", "[SplitString]")
{
    std::string line = "10.0 11.0 12.0  13.0";
    std::vector<int64_t> expected = {10, 11, 12, 13};
    auto result = SplitString<int64_t>::split(line, " ");
    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}

TEST_CASE( "Test split string delimiter", "[SplitString]")
{
    std::string line = "10.0,11.0,12.0,13.0";
    std::vector<int64_t> expected = {10, 11, 12, 13};
    auto result = SplitString<int64_t>::split(line, ",");
    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}

TEST_CASE( "Test split string delimiter with more than one char", "[SplitString]")
{
    std::string line = "10.0,,11.0,,12.0,,13.0";
    std::vector<int64_t> expected = {10, 11, 12, 13};
    auto result = SplitString<int64_t>::split(line, ",,");
    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}

TEST_CASE( "Test split string delimiter with spaces around it", "[SplitString]")
{
    std::string line = "10.0 ,, 11.0 ,, 12.0 ,, 13.0";
    std::vector<int64_t> expected = {10, 11, 12, 13};
    auto result = SplitString<int64_t>::split(line, ",,");
    REQUIRE(result.size() == expected.size());
    REQUIRE(result == expected);
}
