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
        size_t row_count;


        KDTree(size_t row_count);
        ~KDTree();


        std::pair<std::vector<std::pair<size_t, size_t>>, std::vector<bool>>
        search(Query& query);
        //! Seach Nodes relevant to the query
        vector<KDNode*> search_nodes(Query& query, vector<KDNode*>& nodes);

        std::unique_ptr<KDNode> create_node(size_t column, float key, size_t position);

        size_t get_node_count();

        size_t get_max_height();

        size_t get_min_height();

        void draw(std::string path);

        bool sanity_check(Table* table);

    private:

        size_t number_of_nodes = 0;

        void search_recursion(
                KDNode *current,
                size_t lower_limit,
                size_t upper_limit,
                Query& query,
                std::vector<std::pair<size_t, size_t>> &partitions,
                std::vector<bool> &partition_skip,
                std::vector<std::pair<float, float>> partition_borders
                );

        void search_nodes_recursion(
    KDNode *current,
        size_t lower_limit,
        size_t upper_limit,
        Query& query, vector<KDNode*> &nodes
    );
        // Checks if a partition is completely inside the query
        bool partition_inside_query(
                Query& query,
                std::vector<std::pair<float, float>> &partition_borders
                );
        bool sanity_check_recursion(
                Table* table, KDNode* current,
                size_t low, size_t high,
                std::vector<std::pair<float, float>> partition_borders,
                std::vector<std::pair<size_t, size_t>>& partitions
                );
};
#endif // KDTREE_H
