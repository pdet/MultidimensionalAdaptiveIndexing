#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <cstdlib>
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

    Tree  Left;
    Tree  Right;

    int64_t      Height;
    int64_t left_position;
    int64_t right_position;
};

Tree MakeEmpty( Tree T );
int64_t FindLT( int64_t X, Tree T );
int64_t FindLTE( int64_t X, Tree T, int64_t limit );
Position FindMin( Tree T );
Tree Insert( int64_t offset, int64_t X, Tree T );
IntPair FindNeighborsLT( int64_t X, Tree T, int64_t limit );
IntPair FindNeighborsGTE( int64_t X, Tree T, int64_t limit );
int64_t lookup(int64_t X, Tree T);



#endif //AVL_TREE_H
