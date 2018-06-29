//
// Created by PHolanda on 17/12/17.
//

#ifndef MULTIDEMIONSIONALINDEXING_AVL_TREE_H
#define MULTIDEMIONSIONALINDEXING_AVL_TREE_H

#include <cstdlib>
#include "cracker_index.h"
struct int_pair
{
    int64_t first;
    int64_t second;
};
typedef struct int_pair *IntPair;

struct AvlNode;

typedef struct AvlNode *PositionAVL;
typedef struct AvlNode *AvlTree;
AvlTree MakeEmpty( AvlTree T );

int64_t FindLT( int64_t X, AvlTree T );
int64_t FindLTE( int64_t X, AvlTree T, int64_t limit );
PositionAVL FindMin( AvlTree T );
AvlTree Insert( int64_t offset, int64_t X, AvlTree T );
IntPair FindNeighborsLT( int64_t X, AvlTree T, int64_t limit );
IntPair FindNeighborsGTE( int64_t X, AvlTree T, int64_t limit );
int64_t lookup(int64_t X, AvlTree T);


void Print( AvlTree T );



#endif //MULTIDEMIONSIONALINDEXING_AVL_TREE_H