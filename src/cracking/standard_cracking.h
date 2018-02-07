//
// Created by PHolanda on 17/12/17.
//

#ifndef MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
#define MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H

#include "avl_tree.h"
#include "cracker_index.h"
#include "stdio.h"
#include <cstdlib>

AvlTree standardCracking(IndexEntry *&c, int dataSize, AvlTree T, int lowKey, int highKey);

#endif //MULTIDEMIONSIONALINDEXING_STANDARCRACKING_H
