//
// Created by PHolanda on 17/12/17.
// Adapted from Saarland University Uncracked Pieces Simulator
//

#ifndef LIGHTWEIGHTADAPTIVEINDEXING_BINARY_SEARCH_H
#define LIGHTWEIGHTADAPTIVEINDEXING_BINARY_SEARCH_H

#include "../cracking/cracker_index.h"


void *build_binary_tree(IndexEntry *c, int n);

int binary_search(IndexEntry *c, int64_t key, int64_t lower, int64_t upper, bool* foundKey);
int binary_search_lt(IndexEntry *c, int64_t key, int64_t start, int64_t end);
int binary_search_gte(IndexEntry *c, int64_t key, int64_t start, int64_t end);


#endif //LIGHTWEIGHTADAPTIVEINDEXING_BINARY_SEARCH_H
