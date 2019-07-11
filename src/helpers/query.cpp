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
    vector<shared_ptr<Predicate>> predicates;

    Query(vector<float> low, vector<float> high, vector<float> column){
        number_of_predicates = low.size();
        predicates.resize(number_of_predicates);
        for (size_t i = 0; i < number_of_predicates; i++)
        {
            predicates.at(i) = make_unique<Predicate>(
                low.at(i), high.at(i), column.at(i)
            );
        }
    }

    size_t predicate_count(){
        return number_of_predicates;
    }
};
#endif