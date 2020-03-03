#ifndef CRACKING_KD_TREE_MINE_HPP
#define CRACKING_KD_TREE_MINE_HPP

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "abstract_index.hpp"
#include <string>
#include <map>
#include <set>

#include <cstdlib>

template< typename T>
class MyStack{
private:
    T* stack;
    size_t size = 0;
    size_t capacity = 8;
public:
    MyStack(){
        stack = static_cast<T*>(malloc(capacity * sizeof(T)));
    }

    ~MyStack(){
        free(stack);
    }

    void push_back(const T v){
        if(size == capacity){
            capacity = capacity * 2;
            stack = static_cast<T*>(realloc(stack, capacity * sizeof(T)));
        }
        stack[size] = v;
        size++;
    }

    T pop_back(){
        size--;
        auto v = stack[size];
        return v;
    }

    bool empty(){
        return size == 0;
    }
};

class CrackingKDTreeMine : public AbstractIndex
{
    public:
    static const size_t ID = 100;
    CrackingKDTreeMine(std::map<std::string, std::string> config);
    ~CrackingKDTreeMine();

    string name() override{
        return "CrackingKDTreeMine";
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
    unique_ptr<KDTree> index;
    int64_t minimum_partition_size = 100;

    void adapt(
        std::vector<Point> &points,
        std::vector<Edge> &edges
        );

    void insert_point(Point &point, size_t is_right_hand_side);
    void insert_edge(Edge& edge);

    std::vector<Point> query_to_points(Query& query);
    std::vector<Edge> query_to_edges(Query& query);
    Point decompress_edge(size_t compressed_edge, Query& query);

    float max(Point &p1, Point &p2, size_t dimension);
    float min(Point &p1, Point &p2, size_t dimension);

    void crack_point(
            Point &point,   // point to be inserted
            size_t is_right_hand_side,  // if each axis of the point comes
                                        //  from the right side of query
            std::vector<bool> &should_insert, // if the axis should be inserted
            KDNode* current, // node to insert the new point
            int64_t low_position,       // lower position from partition
            int64_t high_position      // upper position from partition
            );

    int64_t next_dim(int64_t start, std::vector<bool> &should_insert);

    bool all_elements_false(std::vector<bool> &v);
};

#endif
