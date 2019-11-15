#ifndef CRACKING_KDTREE_NARROW_H
#define CRACKING_KDTREE_NARROW_H

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <map>
#include <string>

// Query Shell that enables Query Breaking
class QueryShell{
    public:
        vector<bool> crack_left;
        vector<bool> crack_right;
        Query *shell_query;

        QueryShell(Query *query){
            shell_query = query;
            auto n_predicates = query->predicate_count();
            crack_left.resize(n_predicates, true);
            crack_right.resize(n_predicates, true);
        }

        QueryShell(QueryShell *shell){
            shell_query = new Query(*(shell->shell_query));
            crack_left = shell->crack_left;
            crack_right = shell->crack_right;
        }

        ~QueryShell(){
            shell_query = nullptr;
            crack_left.resize(0);
            crack_right.resize(0);
        }
};

class CrackingKDTreeNarrow : public AbstractIndex
{
public:
    CrackingKDTreeNarrow(std::map<std::string, std::string> config);
    ~CrackingKDTreeNarrow();

    string name() override{
        return "Cracking KD-Tree Narrow";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    void draw_index(std::string path) override;
private:
    unique_ptr<KDTree> index;
    int64_t minimum_partition_size = 100;

    // Vectors to simplify the insertion algorithm
    vector<KDNode*> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;
    vector<QueryShell*> queries;

    void insert(Query &query);

    QueryShell* break_query_left(QueryShell *query, KDNode *node);

    QueryShell* break_query_right(QueryShell *query, KDNode *node);

    // Checks the left child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void insert_or_follow_left(KDNode *current, int64_t lower_limit, int64_t upper_limit, QueryShell *query);

    // Checks the right child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void insert_or_follow_right(KDNode *current, int64_t lower_limit, int64_t upper_limit, QueryShell *query);

    unique_ptr<KDNode> insert_new_nodes(QueryShell *query, int64_t lower_limit, int64_t upper_limit);

    // Checks if node's column is inside of query
    bool node_in_query(KDNode *current, QueryShell* shell);

    // If the node's key is greater or equal to the high part of the query
    // Then follow the left child
    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    bool node_greater_equal_query(KDNode *node, QueryShell *query);

    // If the node's key is smaller to the low part of the query
    // Then follow the right child
    //                  Key
    // Data:  |----------!--------|
    // Query:            |-----|
    //                  low   high
    bool node_less_equal_query(KDNode *node, QueryShell *query);
};
#endif // CRACKING_KDTREE_NARROW_H
