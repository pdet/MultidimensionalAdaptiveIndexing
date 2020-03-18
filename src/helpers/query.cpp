#include "predicate.hpp"
#include "query.hpp"
#include <cstdint>
#include <vector>

using namespace std;

Query::Query(vector<float> low, vector<float> high, vector<int64_t> column){
    number_of_predicates = low.size();
    predicates.resize(number_of_predicates);
    for (int64_t i = 0; i < number_of_predicates; i++)
    {
        predicates.at(i) = Predicate(
            low.at(i), high.at(i), column.at(i)
        );
    }
}

Query::Query(const Query& query){
    number_of_predicates = query.number_of_predicates;
    predicates.resize(number_of_predicates);
    for (int64_t i = 0; i < number_of_predicates; i++)
    {
        predicates.at(i) = Predicate(
            query.predicates.at(i).low,
            query.predicates.at(i).high,
            query.predicates.at(i).column
        );
    }
}

Query::Query(){}

int64_t Query::predicate_count(){
    return number_of_predicates;
}

bool Query::covers(vector<pair<float, float>> bounding_box){
    for(auto i = 0; i < number_of_predicates; ++i){
        auto &predicate = predicates.at(i);
        auto &border = bounding_box.at(i);
        if(!(
            predicate.low <= border.first &&
            predicate.high > border.second
        )){
            return false;
        }
    }
    return true;
}
