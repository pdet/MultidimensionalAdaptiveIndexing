#ifndef CRACKING_KDTREE_NARROW
#define CRACKING_KDTREE_NARROW

#include "kd_tree/kd_tree.cpp"
#include "full_scan.cpp"
#include "abstract_index.cpp"

class CrackingKDTreeNarrow : public AbstractIndex
{
private:
    unique_ptr<KDTree> index;
    size_t number_of_nodes = 0;
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

    void adapt_index(const shared_ptr<Query> query){
        // ******************
        auto start = measurements->time();

        insert(query);

        auto end = measurements->time();
        // ******************
        measurements->adaptation_time.push_back(
            Measurements::difference(end, start)
        );
    }

    shared_ptr<Table> range_query(const shared_ptr<Query> query){
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
        measurements->number_of_nodes.push_back(number_of_nodes);
        measurements->max_height.push_back(index->get_max_height());
        measurements->min_height.push_back(index->get_min_height());
        measurements->memory_footprint.push_back(number_of_nodes * sizeof(KDNode));

        return result;
    }
private:

    // Vectors to simplify the insertion algorithm
    vector<shared_ptr<KDNode>> nodes_to_check;
    vector<size_t> lower_limits, upper_limits;

    void insert(shared_ptr<Query> query){
        if(index->root == nullptr){
            auto lower_limit = 0;
            auto upper_limit = table->row_count() - 1;
            bool add_to_right = true;
            bool to_be_root = true;

            shared_ptr<KDNode> current;
            for(auto predicate : query->predicates){
                auto key = predicate->low;
                auto column = predicate->column;
                auto position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(to_be_root){
                        index->root = make_shared<KDNode>(column, key, position, position + 1);
                        lower_limit = position + 1;
                        current = index->root;
                        to_be_root = false;
                        add_to_right = true;
                        number_of_nodes++;
                    }
                    else if(add_to_right){
                        current->right_child = make_shared<KDNode>(column, key, position, position + 1);
                        upper_limit = position;
                        current = current->right_child;
                        add_to_right = !add_to_right;
                        number_of_nodes++;
                    }else{
                        current->left_child = make_shared<KDNode>(column, key, position, position + 1);
                        lower_limit = position + 1;
                        current = current->left_child;
                        add_to_right = !add_to_right;
                        number_of_nodes++;
                    }
                }

                key = predicate->high;
                position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(to_be_root){
                        index->root = make_shared<KDNode>(column, key, position, position + 1);
                        upper_limit= position;
                        current = index->root;
                        to_be_root = false;
                        add_to_right = false;
                        number_of_nodes++;
                    }
                    else if(add_to_right){
                        current->right_child = make_shared<KDNode>(column, key, position, position + 1);
                        upper_limit = position;
                        current = current->right_child;
                        add_to_right = !add_to_right;
                        number_of_nodes++;
                    }else{
                        current->left_child = make_shared<KDNode>(column, key, position, position + 1);
                        lower_limit = position + 1;
                        current = current->left_child;
                        add_to_right = !add_to_right;
                        number_of_nodes++;
                    }
                }
            }
            return;
        }

        nodes_to_check.resize(0);
        lower_limits.resize(0);
        upper_limits.resize(0);

        nodes_to_check.push_back(index->root);
        lower_limits.push_back(0);
        upper_limits.push_back(table->row_count() - 1);
        while(!nodes_to_check.empty()){
            auto current = nodes_to_check.back();
            nodes_to_check.pop_back();

            auto lower_limit = lower_limits.back();
            lower_limits.pop_back();

            auto upper_limit = upper_limits.back();
            upper_limits.pop_back();

            // If current's column is not in query follow both children
            if(!node_in_query(current, query)){
                insert_or_follow_left(current, lower_limit, upper_limit, query);
                insert_or_follow_right(current, lower_limit, upper_limit, query);
            }
            // If the node's key is greater or equal to the high part of the query
            // Then follow the left child
            //                  Key
            // Data:  |----------!--------|
            // Query:      |-----|
            //            low   high
            else if (node_greater_equal_query(current, query)){
                insert_or_follow_left(current, lower_limit, upper_limit, query);
            }
            // If the node's key is smaller to the low part of the query
            // Then follow the right child
            //                  Key
            // Data:  |----------!--------|
            // Query:            |-----|
            //                  low   high
            else if (node_less_equal_query(current, query)){
                insert_or_follow_right(current, lower_limit, upper_limit, query);
            }
            // If the node's key is inside the query
            // Then follow both children
            //                  Key
            // Data:  |----------!--------|
            // Query:         |-----|
            //               low   high
            else{
                insert_or_follow_left(current, lower_limit, upper_limit, query);
                insert_or_follow_right(current, lower_limit, upper_limit, query);
            }
        }
    }

    // Checks the left child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void insert_or_follow_left(shared_ptr<KDNode> current, size_t lower_limit, size_t upper_limit, shared_ptr<Query> query){
        if (current->left_child != nullptr)
        {
            nodes_to_check.push_back(current->left_child);
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->left_position);
        }
        else
        {
            bool add_to_left = true;
            // For each predicate in query
            //  Crack the from low to upper based on its key and column
            //  if the cracking was successful
            //      if(add_to_left)
            //          add node to left
            //          upper_limit = node->upper_position
            //          current = new node
            //      else
            //          add node to right
            //          lower_limit = node->left_position
            //          current = new node
            //
            //      flip add_to_left

            for(auto predicate : query->predicates){
                auto key = predicate->low;
                auto column = predicate->column;
                if(upper_limit - lower_limit < minimum_partition_size)
                    return;
                auto position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(add_to_left){
                        current->left_child = make_shared<KDNode>(column, key, position, position + 1);
                        lower_limit = position + 1;
                        current = current->left_child;
                        number_of_nodes++;
                    }else{
                        current->right_child = make_shared<KDNode>(column, key, position, position + 1);
                        upper_limit = position;
                        current = current->right_child;
                        number_of_nodes++;
                    }
                    add_to_left = !add_to_left;
                }

                key = predicate->high;
                if(upper_limit - lower_limit < minimum_partition_size)
                    return;
                position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(add_to_left){
                        current->left_child = make_shared<KDNode>(column, key, position, position + 1);
                        lower_limit = position + 1;
                        current = current->left_child;
                        number_of_nodes++;
                    }else{
                        current->right_child = make_shared<KDNode>(column, key, position, position + 1);
                        upper_limit = position;
                        current = current->right_child;
                        number_of_nodes++;
                    }
                    add_to_left = !add_to_left;
                }
            }
        }
    }

    // Checks the right child
    // If it is null then we reached a partition
    // Otherwise, we follow it
    void insert_or_follow_right(shared_ptr<KDNode> current, size_t lower_limit, size_t upper_limit, shared_ptr<Query> query){
        if (current->right_child != nullptr)
        {
            nodes_to_check.push_back(current->right_child);
            lower_limits.push_back(current->right_position);
            upper_limits.push_back(upper_limit);
        }
        else
        {
            bool add_to_right = true;
            // For each predicate in query
            //  Crack the from low to upper based on its key and column
            //  if the cracking was successful
            //      if(add_to_right)
            //          add node to right
            //          upper_limit = node->lower_position
            //          current = new node
            //      else
            //          add node to left
            //          lower_limit = node->right_position
            //          current = new node
            //
            //      flip add_to_right

            for(auto predicate : query->predicates){
                auto key = predicate->low;
                auto column = predicate->column;
                if(upper_limit - lower_limit < minimum_partition_size)
                    return;
                auto position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(add_to_right){
                        current->right_child = make_shared<KDNode>(column, key, position, position + 1);
                        upper_limit = position;
                        current = current->right_child;
                        number_of_nodes++;
                    }else{
                        current->left_child = make_shared<KDNode>(column, key, position, position + 1);
                        lower_limit = position + 1;
                        current = current->left_child;
                        number_of_nodes++;
                    }
                    add_to_right = !add_to_right;
                }

                key = predicate->high;
                if(upper_limit - lower_limit < minimum_partition_size)
                    return;
                position = table->CrackTable(lower_limit, upper_limit, key, column);
                if (!(position < lower_limit || position >= upper_limit)){
                    if(add_to_right){
                        current->right_child = make_shared<KDNode>(column, key, position, position + 1);
                        upper_limit = position;
                        current = current->right_child;
                        number_of_nodes++;
                    }else{
                        current->left_child = make_shared<KDNode>(column, key, position, position + 1);
                        lower_limit = position + 1;
                        current = current->left_child;
                        number_of_nodes++;
                    }
                    add_to_right = !add_to_right;
                }
            }
        }
    }

    // Checks if node's column is inside of query
    bool node_in_query(shared_ptr<KDNode> current, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(current->column == query->predicates.at(i)->column)
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
    bool node_greater_equal_query(shared_ptr<KDNode> node, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(node->column == query->predicates.at(i)->column){
                auto high = query->predicates.at(i)->high;
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
    bool node_less_equal_query(shared_ptr<KDNode> node, shared_ptr<Query> query){
        for(size_t i = 0; i < query->predicate_count(); i++)
        {
            if(node->column == query->predicates.at(i)->column){
                auto low = query->predicates.at(i)->low;
                return node->key <= low;
            }
        }
        return false;
    }
};
#endif