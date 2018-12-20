#include "standard_cracking.h"
#include <algorithm>

void StandardCracking::pre_processing(
    vector<int64_t> &ids,
    vector<vector<int64_t> > &columns
){
    cracker_columns.resize(columns.size());
    index.resize(columns.size(), nullptr);
    for(size_t i = 0; i < columns.size(); i++)
    {
        cracker_columns.at(i).resize(ids.size());
        for(size_t j = 0; j < ids.size(); j++)
        {
            cracker_columns.at(i).at(j) = std::make_pair(
                ids.at(j), columns.at(i).at(j)
            );
        }
    }
}

void StandardCracking::partial_index_build(
    vector<array<int64_t, 3> > &query
){

    for(size_t i = 0; i < query.size(); i++)
    {
        int64_t low = query.at(i).at(0);
        int64_t high = query.at(i).at(1);
        int64_t column = query.at(i).at(2);
        index.at(column) = standardCracking(
            cracker_columns.at(column), index.at(column), low, high
        );
    }

}

vector<pair<int64_t, int64_t> > StandardCracking::search(
    vector<array<int64_t, 3> > &query
){}

void StandardCracking::scan(
    vector<pair<int64_t, int64_t> > &partitions
){}


// ###### PRIVATE METHODS #######


void StandardCracking::exchange(CrackerColumn &cracker_column, int64_t x1, int64_t x2)
{
    swap(cracker_column.at(x1), cracker_column.at(x2));
}

int StandardCracking::crackInTwoItemWise(CrackerColumn &cracker_column, int64_t posL, int64_t posH, int64_t med)
{
    int x1 = posL, x2 = posH;
    while (x1 <= x2)
    {
        if (cracker_column[x1].second  < med)
            x1++;
        else
        {
            while (x2 >= x1 && (cracker_column[x2].second >= med))
                x2--;
            if (x1 < x2)
            {
                exchange(cracker_column, x1, x2);
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

IntPair StandardCracking::crackInThreeItemWise(CrackerColumn &cracker_column, int64_t posL, int64_t posH, int64_t low, int64_t high)
{
    int x1 = posL, x2 = posH;
    while (x2 > x1 && cracker_column[x2].second  >= high)
        x2--;
    int x3 = x2;
    while (x3 > x1 && cracker_column[x3].second  >= low)
    {
        if (cracker_column[x3].second >= high)
        {
            exchange(cracker_column, x2, x3);
            x2--;
        }
        x3--;
    }
    while (x1 < x3)
    {
        if (cracker_column[x1].second  < low)
            x1++;
        else
        {
            exchange(cracker_column, x1, x3);
            while (x3 > x1 && cracker_column[x3].second  >= low)
            {
                if (cracker_column[x3].second  >= high)
                {
                    exchange(cracker_column, x2, x3);
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

Tree StandardCracking::standardCracking(
    CrackerColumn &cracker_column,
    Tree &T,
    int64_t lowKey, int64_t highKey
){
    int64_t dataSize = cracker_column.size();
    IntPair p1, p2;

    p1 = FindNeighborsLT(lowKey, T, dataSize - 1);
    p2 = FindNeighborsLT(highKey, T, dataSize - 1);

    IntPair pivot_pair = NULL;

    if (p1->first == p2->first && p1->second == p2->second && lowKey != -1 && highKey != -1)
    {
        pivot_pair = crackInThreeItemWise(cracker_column, p1->first, p1->second, lowKey, highKey);
    }
    else
    {
        // crack in two
        pivot_pair = (IntPair)malloc(sizeof(struct int_pair));
        if(lowKey != -1)
            pivot_pair->first = crackInTwoItemWise(cracker_column, p1->first, p1->second, lowKey);
        if(highKey != -1){
            if(lowKey != -1)
                pivot_pair->second = crackInTwoItemWise(cracker_column, pivot_pair->first, p2->second, highKey);
            else
                pivot_pair->second = crackInTwoItemWise(cracker_column, p1->first, p2->second, highKey);
        }
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