#ifndef CRACKING_KDTREE_BROAD
#define CRACKING_KDTREE_BROAD

#include "kd_tree/kd_tree.cpp"
#include "full_scan.cpp"
#include "abstract_index.cpp"

class CrackingKDTreeBroad : public AbstractIndex
{
private:
    unique_ptr<KDTree> index;
    size_t number_of_nodes = 0;
    const size_t minimum_partition_size = 100;
public:
    CrackingKDTreeBroad(){}
    ~CrackingKDTreeBroad(){}

    string name(){
        return "Cracking KD-Tree Broad";
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

        for(auto predicate : query->predicates){
            insert(predicate->column, predicate->low);
            insert(predicate->column, predicate->high);
        }

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

    void insert(size_t column, float key){
        if(index->root == nullptr){
            // First insertion
            // Crack and insert into root
            size_t lower_limit = 0;
            size_t upper_limit = table->row_count() - 1;
            size_t position = table->CrackTable(lower_limit, upper_limit, key, column);

            if (!(position < lower_limit || position >= upper_limit)){
                index->root = make_unique<KDNode>(column, key, position, position + 1);
                number_of_nodes++;
            }
            return;
        }
        // Search the partitions to crack based on column and key
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

            // Current node shares the same column
            if(current->column == column){
                // Current node is smaller than key to insert, then follow right
                // Current:      (col, key)
                //                         >
                // New:                     (col, k)
                if(current->key < key)
                    follow_or_crack_right(current, column, key, upper_limit);
                // Current node is greater than key to insert
                // Current:       (col, key)
                //              <
                // New:   (col, k)
                else if(current->key > key)
                    follow_or_crack_left(current, column, key, lower_limit);
                // Current node is equal to key to insert
                // Current:      (col, key)
                // New:          (col, key)
                else
                    continue;
            }
            // Does not have the same column, them follow both children
            else{
                follow_or_crack_right(current, column, key, upper_limit);
                follow_or_crack_left(current, column, key, lower_limit);
            }
        }
    }

    void follow_or_crack_right(shared_ptr<KDNode> current, size_t column, float key, float upper_limit){
        // If the right child is null, then we crack that partition
        // Current:      (col, key)
        //              /          \
        // Child:                  null
        if(current->right_child == nullptr){
            if(upper_limit - current->right_position < minimum_partition_size)
                return;
            auto position = table->CrackTable(
                current->right_position, upper_limit,
                key, column
            );
            if(!(position < current->right_position || position >= upper_limit)){
                current->right_child = make_unique<KDNode>(
                    column, key, position, position + 1
                );
                number_of_nodes++;
            }
        }
        // If the right child exists, then we follow it
        // Current:      (col, key)
        //              /          \
        // Child:                 (..., ...)
        else{
            nodes_to_check.push_back(current->right_child);
            lower_limits.push_back(current->right_position);
            upper_limits.push_back(upper_limit);
        }
    }

    void follow_or_crack_left(shared_ptr<KDNode> current, size_t column, float key, float lower_limit){
        // If the left child is null, then we crack that partition
        // Current:      (col, key)
        //              /          \
        // Child:     null
        if(current->left_child == nullptr){
            if(current->left_position - lower_limit < minimum_partition_size)
                return;
            auto position = table->CrackTable(
                lower_limit, current->left_position,
                key, column
            );
            if(!(position < lower_limit || position >= current->left_position)){
                current->left_child = make_unique<KDNode>(
                    column, key, position, position + 1
                );
                number_of_nodes++;
            }
        }
        // If the left child exists, then we follow it
        // Current:      (col, key)
        //              /          \
        // Child: (..., ...)
        else{
            nodes_to_check.push_back(current->left_child);
            lower_limits.push_back(lower_limit);
            upper_limits.push_back(current->left_position);
        }
    }
};
#endif