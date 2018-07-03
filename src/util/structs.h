#ifndef MULTIDEMIONSIONALINDEXING_STRUCTS_H
#define MULTIDEMIONSIONALINDEXING_STRUCTS_H
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

struct RangeQuery {
    int64_t *leftpredicate;
    int64_t *rightpredicate;
};

struct IndexEntry
{
    int64_t m_key;
    int64_t m_rowId;
};


struct CrackerMaps
{
    int leading_column;
    int aux_column;
    vector<int64_t> ids;
    vector<vector<int64_t>> columns;

};


struct CrackerTable
{
    vector<int64_t> ids;
    vector<vector<int64_t>> columns;
};

struct Table
{
    vector<int64_t> ids;
    vector<vector<int64_t>> columns;
    vector<CrackerMaps> crackermaps;
    IndexEntry **crackercolumns;
    CrackerTable crackertable;
};

struct Column {
    int64_t *data;
};


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

#endif //MULTIDEMIONSIONALINDEXING_STRUCTS_H
