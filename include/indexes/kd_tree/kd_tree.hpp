#ifndef KDTREE
#define KDTREE

#include "../../helpers/query.hpp"
#include "kd_node.hpp"

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <limits>
#include <string>

using namespace std;

class KDTree
{
public:

    unique_ptr<KDNode> root; // Root of the tree
    int64_t row_count;


    KDTree(int64_t row_count);
    ~KDTree();

    vector<pair<int64_t, int64_t>> search(Query& query);

    unique_ptr<KDNode> create_node(int64_t column, float key, int64_t position);

    int64_t get_node_count();

    int64_t get_max_height();

    int64_t get_min_height();

    void draw(std::string path);

private:

    int64_t number_of_nodes = 0;

    vector<pair<int64_t, int64_t>> partitions;
    vector<KDNode*> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;
};

#endif // KDTREE
