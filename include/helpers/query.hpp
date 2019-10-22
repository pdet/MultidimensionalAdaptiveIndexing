#ifndef QUERY_H
#define QUERY_H

#include "predicate.hpp"
#include <cstdint>
#include <vector>

using namespace std;

class Query
{
private:
    int64_t number_of_predicates;

public:
    vector<Predicate> predicates;

    Query(vector<float> low, vector<float> high, vector<int64_t> column);

    Query(const Query& query);

    Query();

    int64_t predicate_count();
};
#endif // QUERY_H
