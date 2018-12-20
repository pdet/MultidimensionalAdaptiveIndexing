#ifndef ALGORITHMS_INTERFACE
#define ALGORITHMS_INTERFACE

#include <vector>
#include <array>
#include <utility>
#include <inttypes.h>

using namespace std;

class Algorithm{
    public:
        // Copies the table to the algorithms data structures and initialize
        // the data structures
        // ids: Vector of ID's
        // columns: data in column format
        virtual void pre_processing(
            vector<int64_t> &ids,
            vector<vector<int64_t> > &columns
        ) = 0;

        // Executes the partial index build step on adaptive algorithms
        // query: Set of predicates on each column
        // The first position is the low value
        // The second position is the high value
        // The third position is the column
        virtual void partial_index_build(
            vector<array<int64_t, 3> > &query
        ) = 0;

        // Executes the range search on each required attribute
        // Returns the partitions that need to be scanned
        // query: follows the same pattern described in the partial_index_build™
        virtual vector<pair<int64_t, int64_t> > search(
            vector<array<int64_t, 3> > &query
        ) = 0;

        // Scans the partitions defined on the search process
        // Holds the intermediate results inside the class
        virtual void scan(
            vector<pair<int64_t, int64_t> > &partitions
        ) = 0;
};

#endif //ALGORITHMS_INTERFACE