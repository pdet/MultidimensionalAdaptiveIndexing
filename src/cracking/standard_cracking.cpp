//
// Created by PHolanda on 17/12/17.
//
//
// Adapted from Saarland University Uncracked Pieces Simulator
//
#include "standard_cracking.h"


void exchange(IndexEntry*& c, int64_t x1, int64_t x2){
    IndexEntry tmp = *(c+x1);
    *(c+x1) = *(c+x2);
    *(c+x2) = tmp;
}

int crackInTwoItemWise(IndexEntry*& c, int64_t posL, int64_t posH, int64_t med){
    int x1 = posL, x2 = posH;
    while (x1 <= x2) {
        if(c[x1] < med)
            x1++;
        else {
            while (x2 >= x1 && (c[x2] >= med))
                x2--;
            if(x1 < x2){
                exchange(c, x1,x2);
                x1++;
                x2--;
            }
        }
    }
    if(x1 < x2)
        printf("Not all elements were inspected!");
    x1--;
    if(x1 < 0)
        x1 = 0;
    return x1;
}

IntPair crackInThreeItemWise(IndexEntry *c, int64_t posL, int64_t posH, int64_t low, int64_t high){
    int x1 = posL, x2 = posH;
    while(x2 > x1 && c[x2] >= high)
        x2--;
    int x3 = x2;
    while(x3 > x1 && c[x3] >= low){
        if(c[x3]>=high){
            exchange(c, x2, x3);
            x2--;
        }
        x3--;
    }
    while(x1<x3){
        if(c[x1] < low)
            x1++;
        else{
            exchange(c, x1, x3);
            while(x3 > x1 && c[x3] >= low){
                if(c[x3]>=high){
                    exchange(c, x2, x3);
                    x2--;
                }
                x3--;
            }
        }
    }
    IntPair p = (IntPair) malloc(sizeof(struct int_pair));
    p->first = x3;
    p->second = x2;
    return p;
}


AvlTree standardCracking(IndexEntry* c, int dataSize, AvlTree T, int lowKey, int highKey){
    IntPair p1,p2;

    p1 = FindNeighborsLT(lowKey, T, dataSize-1);
    p2 = FindNeighborsLT(highKey, T, dataSize-1);

    IntPair pivot_pair = NULL ;

    if(p1->first==p2->first && p1->second==p2->second){
        pivot_pair = crackInThreeItemWise(c, p1->first, p1->second, lowKey, highKey);
    }
    else{
        // crack in two
        pivot_pair = (IntPair) malloc(sizeof(struct int_pair));
        pivot_pair->first = crackInTwoItemWise(c, p1->first, p1->second, lowKey);
        pivot_pair->second = crackInTwoItemWise(c, pivot_pair->first, p2->second, highKey);
    }

    T = Insert(pivot_pair->first, lowKey, T);
    T = Insert(pivot_pair->second, highKey, T);

    free(p1);
    free(p2);
    if(pivot_pair) {
        free(pivot_pair);
        pivot_pair = NULL;
    }

    return T;
}
