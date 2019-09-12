#include <catch.hpp>
#include <unordered_set>
#include "index_factory.hpp"

TEST_CASE( "Index Factory returns Full Scan", "[IndexFactory] [FullScan]" ) {
    auto index = IndexFactory::getIndex("Full-Scan");
    INFO("Check if Index Factory returns FullScan pointer");
    REQUIRE(dynamic_cast<FullScan*>(index.get()) != nullptr);
}

