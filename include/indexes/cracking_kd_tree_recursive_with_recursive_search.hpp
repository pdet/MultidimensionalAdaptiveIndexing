#ifndef CRACKING_KD_TREE_RECURSIVE_WITH_RECURSIVE_SEARCH_HPP
#define CRACKING_KD_TREE_RECURSIVE_WITH_RECURSIVE_SEARCH_HPP

#include "kd_tree/kd_tree_recursive.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>
#include <set>

class CrackingKDTreeRecursiveWithRecursiveSearch : public AbstractIndex
{
    public:
    static const size_t ID = 98;
    CrackingKDTreeRecursiveWithRecursiveSearch(std::map<std::string, std::string> config);
    ~CrackingKDTreeRecursiveWithRecursiveSearch();

    string name() override{
        return "CrackingKDTreeRecursiveWithRecursiveSearch";
    }

    void initialize(Table *table_to_copy) override;

    void adapt_index(Query& query) override;

    Table range_query(Query& query) override;

    void draw_index(std::string path) override{
        index->draw(path);
    }

private:
    using Point = std::vector<float>;
    using Edge = std::pair<Point, Point>;
    unique_ptr<KDTreeRecursive> index;
    int64_t minimum_partition_size = 100;

    void adapt(
        std::vector<Point> &points,
        std::vector<Edge> &edges
        );

    void insert_point(Point &point, size_t is_right_hand_side);
    void insert_edge(Edge& edge);
    void insert_edge_recursion(
        KDNode *current,
        Edge& edge,
        int64_t lower_limit,
        int64_t upper_limit,
        size_t pivot_dim
    );

    std::vector<Point> query_to_points(Query& query);
    std::vector<Edge> query_to_edges(Query& query);
    Point decompress_edge(size_t compressed_edge, Query& query);

    float max(Point &p1, Point &p2, size_t dimension);
    float min(Point &p1, Point &p2, size_t dimension);

    int64_t next_dim(int64_t start, std::vector<bool> &should_insert);

    bool all_elements_false(std::vector<bool> &v);
};

#endif
