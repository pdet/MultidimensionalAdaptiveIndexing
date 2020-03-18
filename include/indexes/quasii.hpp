#ifndef QUASII_H
#define QUASII_H

#include "abstract_index.hpp"
#include "full_scan.hpp"
#include "slice.hpp"
#include <map>
#include <string>
#include <cstdint>

class Quasii : public AbstractIndex
{
public:
    static const size_t ID = 6;
    Quasii(std::map<std::string, std::string> config);
    ~Quasii();

    std::string name() override{
        return "Quasii";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    std::unique_ptr<Table> range_query(Query& query) override;

    void draw_index(std::string path) override;

private:
    std::vector<Slice> first_level_slices;

    int64_t last_level_threshold = 2000;
    std::vector<int64_t> dimensions_threshold;

    int64_t count_slices(std::vector<Slice> &slices);

    void adapt(Query& query);

    pair<vector<pair<int64_t, int64_t>>, vector<bool>>
    search(Query& query);
    int64_t binarySearch(const std::vector<Slice> &slice, float key);
    void search_recursion(
            Slice &slice,
            Query &query,
            vector<pair<int64_t, int64_t>> &partitions,
            vector<bool> &partition_skip,
            vector<pair<float, float>> partition_borders
            );
    int64_t predicate_index(int64_t column, Query &query);

    void calculate_level_thresholds();

    void build(std::vector<Slice> &slices, Query &query);

    std::vector<Slice> sliceArtificial(Slice &slice);

    std::vector<Slice> sliceTwoWay(Slice &slice, float key);

    std::vector<Slice> sliceThreeWay(Slice &slice, float low, float high);

    std::vector<Slice> refine(Slice &slice, Predicate &predicate);
};
#endif // QUASII_H
