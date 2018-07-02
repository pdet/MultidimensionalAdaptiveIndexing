#ifndef MULTIDEMIONSIONALINDEXING_AVL_TREE_H
#define MULTIDEMIONSIONALINDEXING_AVL_TREE_H

#include <cstdlib>
#include "../util/structs.h"

Tree MakeEmpty( Tree T );
int64_t FindLT( int64_t X, Tree T );
int64_t FindLTE( int64_t X, Tree T, int64_t limit );
Position FindMin( Tree T );
Tree Insert( int64_t offset, int64_t X, Tree T );
IntPair FindNeighborsLT( int64_t X, Tree T, int64_t limit );
IntPair FindNeighborsGTE( int64_t X, Tree T, int64_t limit );
int64_t lookup(int64_t X, Tree T);



#endif //MULTIDEMIONSIONALINDEXING_AVL_TREE_H