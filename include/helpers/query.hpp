#ifndef QUERY_H
#define QUERY_H

#include "predicate.hpp"
#include <cstdint>
#include <vector>


// Represents a range query with N predicates.
// Each predicate can be:
//      - Range query: x1 <= X < x2
//      - Point query: x1 <= X < x1 * this is how a point query is represented.
class Query
{
private:
    int64_t number_of_predicates;

public:
    std::vector<Predicate> predicates;

    Query(std::vector<float> low, std::vector<float> high, std::vector<int64_t> column);

    Query(const Query& query);

    Query();

    int64_t predicate_count();
};
#endif // QUERY_H
