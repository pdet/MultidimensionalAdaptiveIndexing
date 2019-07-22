#ifndef CRACKING_KDTREE_NARROW
#define CRACKING_KDTREE_NARROW

#include "kd_tree/kd_tree.cpp"
#include "full_scan.cpp"
#include "abstract_index.cpp"

// Query Shell that enables Query Breaking
class QueryShell{
    public:
        vector<bool> crack_left;
        vector<bool> crack_right;
        Query shell_query;

        QueryShell(Query& query){
            shell_query = Query(query);
            auto n_predicates = query.predicate_count();
            crack_left.resize(n_predicates, true);
            crack_right.resize(n_predicates, true);
        }

        QueryShell(){
            shell_query = Query();
            crack_left.resize(0);
            crack_right.resize(0);
        }

        QueryShell(const QueryShell& shell){
            shell_query = Query(shell.shell_query);
            crack_left = shell.crack_left;
            crack_right = shell.crack_right;
        }

        ~QueryShell(){}
};

class CrackingKDTreeNarrow : public AbstractIndex
{
private:
    unique_ptr<KDTree> index;
    const size_t minimum_partition_size = 100;
public:
    CrackingKDTreeNarrow(){}
    ~CrackingKDTreeNarrow(){}

    string name(){
        return "Cracking KD-Tree Narrow";
    }

    void initialize(const shared_ptr<Table> table_to_copy){
        // ******************
        auto start = measurements->time();

        // Copy the entire table, as this is the cracking_table
        table = make_unique<Table>(table_to_copy);

        // Initialize KD-Tree as nullptr
        index = make_unique<KDTree>(table->row_count());

        measurements->initialization_time = measurements->time() - start;
        // ******************
    }

    void adapt_index(Query& query){
        // ******************
        auto start = measurements->time();

        auto shell = QueryShell(query);
        insert(shell);

        auto end = measurements->time();
        // ******************
        measurements->adaptation_time.push_back(
            Measurements::difference(end, start)
        );
    }

    shared_ptr<Table> range_query(Query& query){
        // ******************
        auto start = measurements->time();

        // Search on the index the correct partitions
        auto partitions = index->search(query);

        // Scan the table and returns a materialized view of the result.
        auto result = make_shared<Table>(table->col_count());
        for (auto partition : partitions)
        {
            auto low = partition.first;
            auto high = partition.second;
            FullScan::scan_partition(table, query, low, high, result);
        }

        auto end = measurements->time();
        // ******************
        measurements->query_time.push_back(
            Measurements::difference(end, start)
        );

        // Before returning the result, update the statistics.
        measurements->number_of_nodes.push_back(index->get_node_count());
        measurements->max_height.push_back(index->get_max_height());
        measurements->min_height.push_back(index->get_min_height());
        measurements->memory_footprint.push_back(index->get_node_count() * sizeof(KDNode));

        return result;
    }
private:

    // Vectors to simplify the insertion algorithm
    vector<shared_ptr<KDNode>> nodes_to_check;
    vector<size_t> lower_limits, upper_limits;
    vector<QueryShell> queries;

    void insert(QueryShell& query){
        if(index->root == nullptr){
            index->root = insert_new_nodes(query, 0, table->row_count() - 1);
            return;
        }

        nodes_to_check.resize(0);
        lower_limits.resize(0);
        upper_limits.resize(0);
        queries.resize(0);

        nodes_to_check.push_back(index->root);
        lower_limits.push_back(0);
        upper_limits.push_back(table->row_count() - 1);
        queries.push_back(QueryShell(query));
        while(!nodes_to_check.empty()){
            auto current = nodes_to_check.back();
            nodes_to_check.pop_back();

            auto lower_limit = lower_limits.back();
            lower_limits.pop_back();

            auto upper_limit = upper_limits.back();
            upper_limits.pop_back();

            auto current_query = queries.back();
            queries.pop_back();

            // If current's column is not in query follow both children
            if(!node_in_query(current, current_query)){
                insert_or_follow_left(current, lower_limit, upper_limit, current_query);
                insert_or_follow_right(current, lower_limit, upper_limit, current_query);
            }
            // If the node's key is greater or equal to the high part of the query
            // Then follow the left child
            //                  Key
            // Data:  |----------!--------|
            // Query:      |-----|
            //            low   high
            else if (node_greater_equal_query(current, current_query)){
                insert_or_follow_left(current, lower_limit, upper_limit, query);
            }
            // If the node's key is smaller to the low part of the query
            // Then follow the right child
            //                  Key
            // Data:  |----------!--------|
            // Query:            |-----|
            //                  low   high
            else if (node_less_equal_query(current, current_query)){
                insert_or_follow_right(current, lower_limit, upper_limit, query);
            }
            // If the node's key is inside the query
            // Then follow both children
            //                  Key
            // Data:  |----------!--------|
            // Query:         |-----|
            //               low   high
            else{
                auto query_left = break_query_left(current_query, current);
                auto query_right = break_query_right(current_query, current);
                insert_or_follow_left(current, lower_limit, upper_limit, query_left);
                insert_or_follow_right(current, lower_limit, upper_limit, query_right);
            }
        }
    }

    QueryShell break_query_left(QueryShell& query, shared_ptr<KDNode> node){
        auto query_left = QueryShell(query);
        for(size_t i = 0; i < query.shell_query.predicate_count(); ++i){
            if(query.shell_query.predicates.at(i).column == node->column){
                query_left.crack_right.at(i) = false;
                query_left.shell_query.predicates.at(i).high = node->key;
            }
        }
        return query_left;
    }

    QueryShell break_query_right(QueryShell& query, shared_ptr<KDNode> node){
        auto query_right = QueryShell(query);
        for(size_t i = 0; i < query.shell_query.predicate_count(); ++i){
            if(query.shell_query.predicates.at(i).column == node->column){
                query_right.crack_left.at(i) = false;
                query_right.shell_query.predicates.at(i).low = node->key;
            }
        }
        return query_right;
    }

    // Checks the left child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void insert_or_follow_left(shared_ptr<KDNode> current, size_t lower_limit, size_t upper_limit, QueryShell& query){
        if (current->left_child != nullptr)
        {
            nodes_to_check.push_back(current->left_child);
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->left_position);
            queries.push_back(query);
        }
        else
        {
            current->left_child = insert_new_nodes(query, lower_limit, upper_limit);
        }
    }

    // Checks the right child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void insert_or_follow_right(shared_ptr<KDNode> current, size_t lower_limit, size_t upper_limit, QueryShell& query){
        if (current->right_child != nullptr)
        {
            nodes_to_check.push_back(current->right_child);
            lower_limits.push_back(current->right_position);
            upper_limits.push_back(upper_limit);
            queries.push_back(query);
        }
        else
        {
            current->right_child = insert_new_nodes(query, lower_limit, upper_limit);
        }
    }

    shared_ptr<KDNode> insert_new_nodes(QueryShell& query_shell, size_t lower_limit, size_t upper_limit){
        if(upper_limit - lower_limit < minimum_partition_size)
            return nullptr;

        shared_ptr<KDNode> current = nullptr;
        size_t i = 0;
        for(; i < query_shell.shell_query.predicate_count(); ++i){
            auto predicate = query_shell.shell_query.predicates.at(i);
            auto key = predicate.low;
            auto column = predicate.column;
            auto should_crack_low = query_shell.crack_left.at(i);
            if(should_crack_low){
                auto position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(upper_limit - position + 1 < minimum_partition_size)
                        return nullptr;
                    current = index->create_node(column, key, position);
                    lower_limit = position + 1;
                    query_shell.crack_left.at(i) = false;
                    break;
                }
            }

            auto should_crack_high = query_shell.crack_right.at(i);
            key = predicate.high;
            if(should_crack_high){
                auto position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(position - lower_limit < minimum_partition_size)
                        return nullptr;
                    current = index->create_node(column, key, position);
                    upper_limit = position;
                    query_shell.crack_right.at(i) = false;
                    ++i;
                    break;
                }
            }
        }


        shared_ptr<KDNode> first = current;

        for(; i < query_shell.shell_query.predicate_count(); ++i){
            auto predicate = query_shell.shell_query.predicates.at(i);
            auto key = predicate.low;
            auto column = predicate.column;
            auto should_crack_low = query_shell.crack_left.at(i);
            if(should_crack_low){
                auto position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(upper_limit - position + 1 < minimum_partition_size)
                        return first;
                    current->right_child = index->create_node(column, key, position);
                    upper_limit = position;
                    current = current->right_child;
                }
            }

            auto should_crack_high = query_shell.crack_right.at(i);
            key = predicate.high;
            if(should_crack_high){
                auto position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(position - lower_limit < minimum_partition_size)
                        return first;
                    current->left_child = index->create_node(column, key, position);
                    lower_limit = position + 1;
                    current = current->left_child;
                }
            }
        }
        return first;
    }

    // Checks if node's column is inside of query
    bool node_in_query(shared_ptr<KDNode> current, QueryShell& shell){
        for(size_t i = 0; i < shell.shell_query.predicate_count(); i++)
        {
            if(current->column == shell.shell_query.predicates.at(i).column)
                return true;
        }
        return false;
    }

    // If the node's key is greater or equal to the high part of the query
    // Then follow the left child
    //                  Key
    // Data:  |----------!--------|
    // Query:      |-----|
    //            low   high
    bool node_greater_equal_query(shared_ptr<KDNode> node, QueryShell& query){
        for(size_t i = 0; i < query.shell_query.predicate_count(); i++)
        {
            if(node->column == query.shell_query.predicates.at(i).column){
                auto high = query.shell_query.predicates.at(i).high;
                if(node->key == high){
                    query.crack_right.at(i) = false;
                }
                return high <= node->key;
            }
        }
        return false;
    }

    // If the node's key is smaller to the low part of the query
    // Then follow the right child
    //                  Key
    // Data:  |----------!--------|
    // Query:            |-----|
    //                  low   high
    bool node_less_equal_query(shared_ptr<KDNode> node, QueryShell& query){
        for(size_t i = 0; i < query.shell_query.predicate_count(); i++)
        {
            if(node->column == query.shell_query.predicates.at(i).column){
                auto low = query.shell_query.predicates.at(i).low;
                if(node->key == low){
                    query.crack_left.at(i) = false;
                }
                return node->key <= low;
            }
        }
        return false;
    }
};
#endif