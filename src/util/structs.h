#ifndef MULTIDEMIONSIONALINDEXING_STRUCTS_H
#define MULTIDEMIONSIONALINDEXING_STRUCTS_H
#include <queue>
#include <climits>
#include <algorithm>
#include <string>

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
    vector<Slice> *refined_slices;
    Slice() : level(0), data_offset_begin(0), data_offset_end(COLUMN_SIZE), box_begin(0), box_end(COLUMN_SIZE){
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
    vector<vector<CrackerMaps> > crackermaps;
    MapSet mapset;
    IndexEntry **crackercolumns;
    CrackerTable crackertable;
};


#endif //MULTIDEMIONSIONALINDEXING_STRUCTS_H
