#ifndef QUASII_H
#define QUASII_H

#include "abstract_index.hpp"
#include "full_scan.hpp"
#include "slice.hpp"

using namespace std;

class Quasii : public AbstractIndex
{
public:
    Quasii();
    ~Quasii();

    string name() override{
        return "Quasii";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

private:
    vector<Slice> first_level_slices;

    const size_t last_level_threshold = 2000;
    vector<size_t> dimensions_threshold;

    size_t count_slices(vector<Slice> &slices);

    void adapt(Query& query);

    vector<pair<size_t, size_t> > search(Query& query);
    size_t binarySearch(const vector<Slice> &slice, float key);

    size_t predicate_index(size_t column, Query &query);

    bool column_in_query(size_t column, Query &query);

    Predicate predicate_on_column(size_t column, Query& query);

    void calculate_level_thresholds();

    void build(vector<Slice> &slices, Query &query);

    vector<Slice> sliceArtificial(Slice &slice);

    vector<Slice> sliceTwoWay(Slice &slice, float key);

    vector<Slice> sliceThreeWay(Slice &slice, float low, float high);

    vector<Slice> refine(Slice &slice, Predicate &predicate);
};
#endif // QUASII_H
