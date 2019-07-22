#ifndef QUERY_H
#define QUERY_H

#include "predicate.hpp"
#include <cstdint>
#include <vector>

using namespace std;

class Query
{
private:
    size_t number_of_predicates;

public:
    vector<Predicate> predicates;

    Query(vector<float> low, vector<float> high, vector<float> column);

    Query(const Query& query);

    Query();

    size_t predicate_count();
};
#endif // QUERY_H