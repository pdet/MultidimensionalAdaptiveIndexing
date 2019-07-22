#ifndef QUERY
#define QUERY

#include "predicate.cpp"
#include <cstdint>
#include <vector>

using namespace std;

class Query
{
private:
    size_t number_of_predicates;

public:
    vector<Predicate> predicates;

    Query(vector<float> low, vector<float> high, vector<float> column){
        number_of_predicates = low.size();
        predicates.resize(number_of_predicates);
        for (size_t i = 0; i < number_of_predicates; i++)
        {
            predicates.at(i) = Predicate(
                low.at(i), high.at(i), column.at(i)
            );
        }
    }

    Query(const Query& query){
        number_of_predicates = query.number_of_predicates;
        predicates.resize(number_of_predicates);
        for (size_t i = 0; i < number_of_predicates; i++)
        {
            predicates.at(i) = Predicate(
                query.predicates.at(i).low,
                query.predicates.at(i).high,
                query.predicates.at(i).column
            );
        }
    }

    Query(){}

    size_t predicate_count(){
        return number_of_predicates;
    }
};
#endif