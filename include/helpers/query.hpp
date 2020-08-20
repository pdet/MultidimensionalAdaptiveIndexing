#ifndef QUERY_H
#define QUERY_H

#include "predicate.hpp"
#include <cstdint>
#include <vector>


// Represents a range query with N predicates.
// Each predicate can be:
//      - Range query: x1 <= X <= x2
class Query
{
private:
    int64_t number_of_predicates;

public:
    std::vector<Predicate> predicates;

    Query(std::vector<float> low, std::vector<float> high, std::vector<size_t> column);

    Query(std::vector<std::pair<float, float>> lows_and_highs);

    Query(const Query& query);

    Query();
    std::vector<bool> covers(std::vector<std::pair<float, float>> bounding_box);
    size_t predicate_count();
};
#endif // QUERY_H
