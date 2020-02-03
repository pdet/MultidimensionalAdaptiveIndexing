#include "all_hyperplanes_narrow.hpp"
#include <vector>
#include <limits>

AllHyperplanesNarrow::AllHyperplanesNarrow(std::map<std::string, std::string> config)
    : AbstractCrackingKDTree(config) {}

AllHyperplanesNarrow::~AllHyperplanesNarrow(){}

void AllHyperplanesNarrow::adapt_index(Query& query){
    auto number_of_predicates = query.predicates.size();
    // Create false predicates with -inf and +inf
    std::vector<float> lows(
            number_of_predicates, std::numeric_limits<float>::lowest()
            );
    std::vector<float> highs(
            number_of_predicates, std::numeric_limits<float>::infinity()
            );
    // Copy the columns from the query
    std::vector<int64_t> cols (number_of_predicates);
    for(auto& predicate : query.predicates)
        cols.push_back(predicate.column);

    auto temporary_query = Query(lows, highs, cols);

    for(auto& predicate : query.predicates){
        insert(temporary_query, predicate.column, predicate.low);
        insert(temporary_query, predicate.column, predicate.high);
    }
};

