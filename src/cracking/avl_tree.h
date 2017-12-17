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

typedef int64_t ElementType;

struct AvlNode;

typedef struct AvlNode *PositionAVL;
typedef struct AvlNode *AvlTree;
AvlTree MakeEmpty( AvlTree T );

int64_t FindLT( ElementType X, AvlTree T );
int64_t FindLTE( ElementType X, AvlTree T, ElementType limit );
PositionAVL FindMin( AvlTree T );
AvlTree Insert( int64_t offset, ElementType X, AvlTree T );
IntPair FindNeighborsLT( ElementType X, AvlTree T, ElementType limit );
IntPair FindNeighborsGTE( ElementType X, AvlTree T, ElementType limit );
int64_t lookup(ElementType X, AvlTree T);


void Print( AvlTree T );



#endif //LIGHTWEIGHTADAPTIVEINDEXING_AVLTREE_H