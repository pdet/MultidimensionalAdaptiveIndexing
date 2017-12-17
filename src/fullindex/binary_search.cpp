//
// Created by PHolanda on 17/12/17.
// Adapted from Saarland University Uncracked Pieces Simulator
//

#include "binary_search.h"



void *build_binary_tree(IndexEntry *c, int64_t n)
{
    // C is already sorted, so just return it.
    return c;
}

int binary_search(IndexEntry *c, int64_t key, int64_t lower, int64_t upper, bool* foundKey ){

    *foundKey = false;
    // binary search: iterative version with early termination
    while(lower <= upper) {
        int middle = (lower + upper) / 2;
        IndexEntry middleElement = c[middle];

        if(middleElement < key) {
            lower = middle + 1;
        }
        else if(middleElement > key) {
            upper = middle - 1;
        }
        else {
            *foundKey = true;
            return middle;
        }
    }
    return upper;
}

int binary_search_lt(IndexEntry *c, int64_t key, int64_t start, int64_t end){
    bool found = false;
    int pos = binary_search(c, key, start, end, &found);
    if(found)
    {
        // The element in "c[pos] < key". So iterating until finding the right pos.
        while(--pos >= start && c[pos] == key);
    }
    // If not found then pos points either to the end or
    // to the last element which which satisfies the c[pos] < key
    return pos;
}

int binary_search_gte(IndexEntry *c, int64_t key, int64_t start, int64_t end){
    bool found = false;
    int pos = binary_search(c, key, start, end, &found);
    if(found)
    {
        // The element in c[pos] >= key.
        while(--pos >= start && c[pos] == key);
    }
    // To get to the first c[pos] == key element.
    ++pos;
    return pos;
}


