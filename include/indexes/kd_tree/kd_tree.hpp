#ifndef KDTREE_H
#define KDTREE_H

#include "query.hpp"
#include "table.hpp"
#include "kd_node.hpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <string>

class KDTree
{
    public:

        unique_ptr<KDNode> root; // Root of the tree
        int64_t row_count;


        KDTree(int64_t row_count);
        ~KDTree();


        std::pair<std::vector<std::pair<int64_t, int64_t>>, std::vector<bool>>
        search(Query& query);

        std::unique_ptr<KDNode> create_node(int64_t column, float key, int64_t position);

        int64_t get_node_count();

        int64_t get_max_height();

        int64_t get_min_height();

        void draw(std::string path);

        bool sanity_check(Table* table);

    private:

        int64_t number_of_nodes = 0;

        void search_recursion(
                KDNode *current,
                int64_t lower_limit,
                int64_t upper_limit,
                Query& query,
                std::vector<std::pair<int64_t, int64_t>> &partitions,
                std::vector<bool> &partition_skip,
                std::vector<std::pair<float, float>> partition_borders
                );
        // Checks if a partition is completely inside the query
        bool partition_inside_query(
                Query& query,
                std::vector<std::pair<float, float>> &partition_borders
                );
        bool sanity_check_recursion(
                Table* table, KDNode* current,
                int64_t low, int64_t high,
                std::vector<std::pair<float, float>> partition_borders
                );
};
#endif // KDTREE_H
