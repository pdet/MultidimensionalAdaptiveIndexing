#ifndef BRANCHLESS_FULL_SCAN_H
#define BRANCHLESS_FULL_SCAN_H

#include "../interface.h"

class BranchlessFullScan : public Algorithm {
    public:
        // Copies the table to the algorithms data structures and initialize
        // the data structures
        // ids: Vector of ID's
        // columns: data in column format
        void pre_processing(
            vector<int64_t> &ids,
            vector<vector<int64_t> > &columns
        );

        // Executes the partial index build step on adaptive algorithms
        // query: Set of predicates on each column
        // The first position is the low value
        // The second position is the high value
        // The third position is the column
        void partial_index_build(
            vector<array<int64_t, 3> > &query
        );

        // Executes the range search on each required attribute
        // Stores internally the partitions that need to be scanned
        // query: follows the same pattern described in the partial_index_build™
        void search(
            vector<array<int64_t, 3> > &query
        );

        // Scans the partitions defined on the search process
        // Holds the intermediate results inside the class
        void scan();

        // For the algorithms that need to intersect the partial results
        void intersect();

        vector<int64_t> get_result();
    private:
        size_t data_size;
        size_t number_of_columns;
        vector<int64_t> ids;
        vector<int64_t*> columns;
        vector<int64_t> resulting_ids;

        int64_t select_rq_scan_sel_vec(
            int64_t*__restrict__ sel,
            int64_t*__restrict__ col,
            int64_t keyL, int64_t keyH, int64_t n
        );
        int64_t select_rq_scan_new (
            int64_t*__restrict__ sel,
            int64_t*__restrict__ col,
            int64_t keyL, int64_t keyH, int64_t n
        );
};

#endif // BRANCHLESS_FULL_SCAN_H
