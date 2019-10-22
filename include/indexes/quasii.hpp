#ifndef QUASII_H
#define QUASII_H

#include "abstract_index.hpp"
#include "full_scan.hpp"
#include "slice.hpp"
#include <map>
#include <string>
#include <cstdint>

using namespace std;

class Quasii : public AbstractIndex
{
public:
    Quasii(std::map<std::string, std::string> config);
    ~Quasii();

    string name() override{
        return "Quasii";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

private:
    vector<Slice> first_level_slices;

    int64_t last_level_threshold = 2000;
    vector<int64_t> dimensions_threshold;

    int64_t count_slices(vector<Slice> &slices);

    void adapt(Query& query);

    vector<pair<int64_t, int64_t> > search(Query& query);
    int64_t binarySearch(const vector<Slice> &slice, float key);

    int64_t predicate_index(int64_t column, Query &query);

    bool column_in_query(int64_t column, Query &query);

    Predicate predicate_on_column(int64_t column, Query& query);

    void calculate_level_thresholds();

    void build(vector<Slice> &slices, Query &query);

    vector<Slice> sliceArtificial(Slice &slice);

    vector<Slice> sliceTwoWay(Slice &slice, float key);

    vector<Slice> sliceThreeWay(Slice &slice, float low, float high);

    vector<Slice> refine(Slice &slice, Predicate &predicate);
};
#endif // QUASII_H
