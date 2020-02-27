#include <catch.hpp>
#include <unordered_set>
#include "index_factory.hpp"
#include <algorithm>

TEST_CASE("No Repeating Index IDs in Index Factory", "[IndexFactory]") {
    auto ids = IndexFactory::algorithmIDs();
    for(size_t i = 0; i < ids.size(); ++i){
        for(size_t j = i + 1; j < ids.size(); ++j){
            REQUIRE(ids.at(i) != ids.at(j));
        }
    }
}

