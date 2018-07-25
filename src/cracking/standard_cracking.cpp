#include "standard_cracking.h"
#include <array>

using namespace std;

extern int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS;
void exchange(IndexEntry *&c, int64_t x1, int64_t x2)
{
    IndexEntry tmp = *(c + x1);
    *(c + x1) = *(c + x2);
    *(c + x2) = tmp;
}

int crackInTwoItemWise(IndexEntry *&c, int64_t posL, int64_t posH, int64_t med)
{
    int x1 = posL, x2 = posH;
    while (x1 <= x2)
    {
        if (c[x1].m_key  < med)
            x1++;
        else
        {
            while (x2 >= x1 && (c[x2].m_key >= med))
                x2--;
            if (x1 < x2)
            {
                exchange(c, x1, x2);
                x1++;
                x2--;
            }
        }
    }
    if (x1 < x2)
        printf("Not all elements were inspected!");
    x1--;
    if (x1 < 0)
        x1 = 0;
    return x1;
}

IntPair crackInThreeItemWise(IndexEntry *c, int64_t posL, int64_t posH, int64_t low, int64_t high)
{
    int x1 = posL, x2 = posH;
    while (x2 > x1 && c[x2].m_key  >= high)
        x2--;
    int x3 = x2;
    while (x3 > x1 && c[x3].m_key  >= low)
    {
        if (c[x3].m_key >= high)
        {
            exchange(c, x2, x3);
            x2--;
        }
        x3--;
    }
    while (x1 < x3)
    {
        if (c[x1].m_key  < low)
            x1++;
        else
        {
            exchange(c, x1, x3);
            while (x3 > x1 && c[x3].m_key  >= low)
            {
                if (c[x3].m_key  >= high)
                {
                    exchange(c, x2, x3);
                    x2--;
                }
                x3--;
            }
        }
    }
    IntPair p = (IntPair)malloc(sizeof(struct int_pair));
    p->first = x3;
    p->second = x2;
    return p;
}

Tree standardCracking(IndexEntry *&c, int dataSize, Tree T, int lowKey, int highKey)
{
    IntPair p1, p2;

    p1 = FindNeighborsLT(lowKey, T, dataSize - 1);
    p2 = FindNeighborsLT(highKey, T, dataSize - 1);

    IntPair pivot_pair = NULL;

    if (p1->first == p2->first && p1->second == p2->second && lowKey != -1 && highKey != -1)
    {
        pivot_pair = crackInThreeItemWise(c, p1->first, p1->second, lowKey, highKey);
    }
    else
    {
        // crack in two
        pivot_pair = (IntPair)malloc(sizeof(struct int_pair));
        if(lowKey != -1)
            pivot_pair->first = crackInTwoItemWise(c, p1->first, p1->second, lowKey);
        if(highKey != -1)
            pivot_pair->second = crackInTwoItemWise(c, pivot_pair->first, p2->second, highKey);
    }

    if(lowKey != -1)
        T = Insert(pivot_pair->first, lowKey, T);
    if(highKey != -1)
        T = Insert(pivot_pair->second, highKey, T);

    free(p1);
    free(p2);
    if (pivot_pair)
    {
        free(pivot_pair);
        pivot_pair = NULL;
    }

    return T;
}


void cracking_pre_processing(Table *table, Tree * T){
     for (size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
    {
        table->crackercolumns[j] = (IndexEntry *)malloc(COLUMN_SIZE * sizeof(IndexEntry));
        for (size_t i = 0; i < COLUMN_SIZE; ++i)
        {
            table->crackercolumns[j][i].m_key = table->columns[j][i];
            table->crackercolumns[j][i].m_rowId = table->ids[i];
        }
    }
    for (size_t k = 0; k < NUMBER_OF_COLUMNS; ++k)
    {
        T[k] = NULL;
    }
}

void cracking_partial_built(Table *table, Tree * T, vector<array<int64_t, 3>>  *rangequeries){
    for (size_t query_num = 0; query_num < rangequeries->size(); query_num++){
        int64_t low = rangequeries->at(query_num).at(0);
        int64_t high = rangequeries->at(query_num).at(1);
        int64_t col = rangequeries->at(query_num).at(2);
        T[col] = standardCracking(table->crackercolumns[col],COLUMN_SIZE,T[col],low,high);
    }
}

void cracking_index_lookup(Tree * T, vector<array<int64_t, 3>> *rangequeries,vector<pair<int,int>>  *offsets){
    for (size_t query_num = 0; query_num < rangequeries->size(); query_num++){
        int64_t low = rangequeries->at(query_num).at(0);
        int64_t high = rangequeries->at(query_num).at(1);
        int64_t col = rangequeries->at(query_num).at(2);
        IntPair p1, p2;
        if(low == -1){
            p1 = (IntPair) malloc(sizeof(struct int_pair));
            p1->first = 0;
        }
        else{
            p1 = FindNeighborsGTE(low, T[col], COLUMN_SIZE - 1);
        }
        if(high == -1){
            p2 = (IntPair) malloc(sizeof(struct int_pair));
            p2->second = COLUMN_SIZE - 1;
        }
        else{
            p2 = FindNeighborsLT(high, T[col], COLUMN_SIZE - 1);
        }
        offsets->push_back(make_pair(p1->first, p2->second));
    }
    
}

void cracking_intersection(Table *table,vector<pair<int,int>>  *offsets, vector<vector<bool>> *bitmaps, vector<int64_t> * result){
    for (size_t i = 0; i < offsets->size(); i ++){
        bitmaps->at(i) = vector<bool>(COLUMN_SIZE); 
        create_bitmap(table->crackercolumns[i], offsets->at(i).first, offsets->at(i).second, bitmaps->at(i));
    }
    *result = join_bitmaps(bitmaps);
}
