#ifndef MULTIDEMIONSIONALINDEXING_KD_TREE_H
#define MULTIDEMIONSIONALINDEXING_KD_TREE_H

#include "stdio.h"
#include "../util/util.h"
#include <cstdlib>
#include <stdlib.h>
using namespace std;

Tree FullTree(Table &table);

void freeTree(Tree tree);

void Print( Tree T );

#endif //MULTIDEMIONSIONALINDEXING_KD_TREE_H