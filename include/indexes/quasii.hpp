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

    string name(){
        return "Quasii";
    }

    void initialize(const shared_ptr<Table> table_to_copy);

    void adapt_index(Query& query);

    shared_ptr<Table> range_query(Query& query);

private:

    size_t minimum_partition_size = 100;
    size_t number_of_slices = 0;
    vector<Slice> slices;

    const size_t last_level_threshold = 2000;
    vector<size_t> dimensions_threshold;

    void adapt(Query& query);

    vector<pair<size_t, size_t> > search(Query& query);
    size_t binarySearch(const vector<Slice> &S, float key);

    size_t predicate_index(size_t column, Query &query);

    bool column_in_query(size_t column, Query &query);

    Predicate predicate_on_column(size_t column, Query& query);

    void calculate_level_thresholds();

    void build(vector<Slice> &Slices, Query &rangequeries);

    vector<Slice> sliceArtificial(Slice &S);

    vector<Slice> sliceTwoWay(Slice &S, float key);

    vector<Slice> sliceThreeWay(Slice &S, float low, float high);

    vector<Slice> refine(Slice &slice, Query &rangequeries);
};
#endif // QUASII_H
