#ifndef CRACKING_KD_TREE_H
#define CRACKING_KD_TREE_H

#include "../interface.h"

struct Node;

typedef struct Node *Position;
typedef struct Node *Tree;

struct Node
{
    int64_t Element;
    int64_t offset;
    int64_t column;

    Tree  Left;
    Tree  Right;

    int64_t left_position;
    int64_t right_position;
};

class CrackingKdTree : public Algorithm {
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
        void scan(vector<array<int64_t, 3> > &query);

        // For the algorithms that need to intersect the partial results
        void intersect();

        vector<int64_t> get_result();
    private:
        int64_t threshold;
        size_t data_size;
        size_t number_of_columns;
        vector<int64_t> ids;
        vector<vector<int64_t> > columns;
        vector<int64_t> resulting_ids;
        vector <pair<int64_t, int64_t> > offsets;
        Tree index;

        void exchange(int64_t x1, int64_t x2);
        int64_t crack_table(int64_t low, int64_t high, int64_t element, int64_t c);
        Tree create_node(int64_t column, int64_t element, int64_t left_position, int64_t right_position);
        Tree check_left_side(Tree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit);
        Tree check_right_side(Tree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit);
        void insert_into_root(Tree &tree, int64_t column, int64_t element);
        void insert(Tree &tree, int64_t column, int64_t element);
        bool node_in_query(Tree current, vector<array<int64_t, 3> > &query);
        bool node_greater_equal_query(Tree node, vector<array<int64_t, 3> > &query);
        bool node_less_equal_query(Tree node, vector<array<int64_t, 3> > &query);
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

#endif // CRACKING_KD_TREE_H
