#ifndef MULTIDEMIONSIONALINDEXING_STRUCTS_H
#define MULTIDEMIONSIONALINDEXING_STRUCTS_H
#include <queue>
#include <climits>
#include <algorithm>
#include <string>
#include <array>

using namespace std;

extern int64_t COLUMN_SIZE;


// ########### BST Structures #############

struct int_pair
{
    int64_t first;
    int64_t second;
};
typedef struct int_pair *IntPair;
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

    int64_t      Height;
    int64_t left_position;
    int64_t right_position;
};

// ########### File Structures #############

struct Column // Input/Output for UniformRandom Benchmark
{ 
    int64_t *data;
};

struct RangeQuery // Input/Output for UniformRandom Benchmark
{
    int64_t *leftpredicate;
    int64_t *rightpredicate;
};

struct IndexEntry  // Standard Cracking
{
    int64_t m_key;
    int64_t m_rowId;
};


struct CrackerMaps // Sideways Cracking
{
    int leading_column;
    int aux_column;
    vector<int64_t> ids;
    vector<vector<int64_t> > columns;
    int64_t crack_position;

};

struct CrackerSets // Sideways Cracking
{
    int leading_column;
    vector<CrackerMaps> crackermaps;
    vector<array<int64_t, 3>> rangequeries;
    vector<Tree> T;
};

struct PartialMaps // Partial Sideways Cracking
{
    vector<int64_t> leading_column;
    vector<int64_t> aux_column;
    Tree T;

};

struct ChunkMap // Partial Sideways Cracking
{
    vector<int64_t> ids;
    vector<int64_t> leading_column;
    // vector<IntPair> fetched;
    // vector<IntPair> log;
    Tree T;

};
struct Slice;
typedef struct Slice Slice;

struct Slice // Quasii
{
    int level;

    int64_t data_offset_begin;
    int64_t data_offset_end;
    int64_t box_begin;
    int64_t box_end;
    vector<Slice> children;
    // box_end shouldn't be COLUMN_SIZE, it should be the biggest possible value?
    Slice() : level(0), data_offset_begin(0), data_offset_end(COLUMN_SIZE - 1), box_begin(0), box_end(COLUMN_SIZE){
    }

    Slice(int l, int64_t offset_begin, int64_t offset_end, int64_t b_begin, int64_t b_end){
        level = l;
        data_offset_begin = offset_begin;
        data_offset_end = offset_end;
        box_begin = b_begin;
        box_end = b_end;
        children = vector<Slice>();
    }

    bool isBottomLevel(int leaf_level){
        return level == leaf_level;
    }

    bool intersects(int64_t low, int64_t high){
        return (
            (box_begin <= low && low < box_end) ||
            (box_begin < high && high <= box_end) ||
            (low <= box_begin && box_end <= high)
        );
    }

    bool bigger_than_threshold(int64_t t){
        return (data_offset_end - data_offset_begin) > t;
    }

    void copy_slice(const Slice s){
        level = s.level;
        data_offset_begin = s.data_offset_begin;
        data_offset_end = s.data_offset_end;
        box_begin = s.box_begin;
        box_end = s.box_end;
        children = s.children;
    }

    bool equal(const Slice s){
        return (
            level == s.level &&
            data_offset_begin == s.data_offset_begin &&
            data_offset_end == s.data_offset_end &&
            box_begin == s.box_begin &&
            box_end == s.box_end
        );
    }
};

 
struct MapSet // Partial Sideways Cracking
{
    ChunkMap chunkmap;
    vector<PartialMaps> partialMaps;

};

struct CrackerTable // Cracking KD-Tree
{
    vector<int64_t> ids;
    vector<vector<int64_t> > columns;
    vector<vector<string> > s_columns;
};

struct Table
{
    vector<int64_t> ids;
    vector<vector<int64_t> > columns;
    vector<vector<string> > s_columns;
    MapSet mapset;
    IndexEntry **crackercolumns;
    CrackerTable crackertable;
};


#endif //MULTIDEMIONSIONALINDEXING_STRUCTS_H
