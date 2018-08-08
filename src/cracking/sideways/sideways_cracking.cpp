#include "sideways_cracking.h"

using namespace std;

extern int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS;

void exchange(CrackerMaps *t, int64_t x1, int64_t x2)
{
    if (x1 == x2)
        return;
    int64_t tmp;
    tmp = t->ids.at(x1);
    t->ids.at(x1) = t->ids.at(x2);
    t->ids.at(x2) = tmp;

    for (size_t i = 0; i < 2; i++)
    {
        tmp = t->columns.at(i).at(x1);
        t->columns.at(i).at(x1) = t->columns.at(i).at(x2);
        t->columns.at(i).at(x2) = tmp;
    }
}

int64_t crack_map(CrackerMaps *map, int64_t low, int64_t high, int64_t element)
{
    int64_t x1 = low;
    int64_t x2 = high;

    while (x1 <= x2)
    {
        if (map->columns.at(0).at(x1) < element)
            x1++;
        else
        {
            while (x2 >= x1 && (map->columns.at(0).at(x2) >= element))
                x2--;
            if (x1 < x2)
            {
                exchange(map, x1, x2);
                x1++;
                x2--;
            }
        }
    }
    if (x1 < x2)
        printf("Not all elements were inspected!");
    x1--;
    return x1;
}

Tree sideways_cracking(CrackerMaps *map, Tree T, int64_t lowKey, int64_t highKey)
{
    IntPair p1, p2;

    if(lowKey != -1)
        p1 = FindNeighborsLT(lowKey, T, COLUMN_SIZE - 1);
    
    if(highKey != -1)
        p2 = FindNeighborsLT(highKey, T, COLUMN_SIZE - 1);

    IntPair pivot_pair = NULL;
  
    // crack in two
    pivot_pair = (IntPair)malloc(sizeof(struct int_pair));
    pivot_pair->first = 0;

    if(lowKey != -1)
        pivot_pair->first = crack_map(map, p1->first, p1->second, lowKey);
    
    if(highKey != -1)
        pivot_pair->second = crack_map(map, pivot_pair->first, p2->second, highKey);

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

void sideways_cracking_pre_processing(Table *table, Tree * T){
    table->crackermaps.resize(NUMBER_OF_COLUMNS);
    for (size_t i = 0; i < NUMBER_OF_COLUMNS ; ++i)
    {
        table->crackermaps.at(i).resize(NUMBER_OF_COLUMNS);
        for(size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
        {
            CrackerMaps map;
            map.leading_column = i;
            map.aux_column = j;
            map.ids=table->ids;
            map.columns.push_back(table->columns.at(i));  // Always get most selective columns as leading column
            map.columns.push_back(table->columns.at(j));  
            table->crackermaps.at(i).at(j) = map;
        }
        T[i] = NULL;
    }
}

void sideways_cracking_partial_built(Table *table, Tree * T, vector<array<int64_t, 3>>  *rangequeries){
    int64_t low = rangequeries->at(0).at(0);
    int64_t high = rangequeries->at(0).at(1);
    int64_t col = rangequeries->at(0).at(2);
    for (size_t i = 1; i < table->crackermaps.at(col).size(); i ++){
        IntPair p1, p2;
        IntPair pivot_pair = (IntPair)malloc(sizeof(struct int_pair));

        pivot_pair->first = 0;

        if(low != -1){
            p1 = FindNeighborsLT(low, T[col], COLUMN_SIZE - 1);
            pivot_pair->first = crack_map(&table->crackermaps.at(col).at(i), p1->first, p1->second, low);
        }

        if(high != -1){
            p2 = FindNeighborsLT(high, T[col], COLUMN_SIZE - 1);
            pivot_pair->second = crack_map(&table->crackermaps.at(col).at(i), pivot_pair->first, p2->second, high);
        }
    }

    T[col] = sideways_cracking(&table->crackermaps.at(col).at(0), T[col], low, high);

}

void sideways_cracking_index_lookup(Tree * T, vector<array<int64_t, 3> >  *rangequeries,vector<pair<int,int>>  *offsets){
    int64_t low = rangequeries->at(0).at(0);
    int64_t high = rangequeries->at(0).at(1);
    int64_t col = rangequeries->at(0).at(2);
    IntPair p1, p2;
    if(low != -1)
        p1 = FindNeighborsGTE(low, T[col], COLUMN_SIZE - 1);
    else
        p1->first = 0;
    
    if(high != -1)
        p2 = FindNeighborsLT(high, T[col], COLUMN_SIZE - 1);
    else
        p2->second = COLUMN_SIZE - 1;
    offsets->push_back(make_pair(p1->first, p2->second));
}

void scan_maps(CrackerMaps *map, vector<bool> &bitmap, int lowOffset, int highOffset, int lowKey, int highKey){
    for(size_t i = 0; i < highOffset - lowOffset + 1; ++i)
        if(bitmap[i])
            if(!(
                ((lowKey <= map->columns.at(1).at(lowOffset+i)) || (lowKey == -1)) &&
                ((map->columns.at(1).at(lowOffset+i) < highKey) || (highKey == -1))
                ))
                bitmap[i] = 0;
}

void sideways_cracking_scan(Table *table, vector<array<int64_t, 3> >  *rangequeries,vector<pair<int,int>> *offsets, vector<int64_t> * result){
    int64_t leading_col = rangequeries->at(0).at(2);
    vector<bool> bitmap(offsets->at(0).second - offsets->at(0).first + 1); 
    for(size_t i = 0; i < offsets->at(0).second - offsets->at(0).first  + 1; ++i)
        bitmap[i]=1;
    for (size_t query_num = 0; query_num < rangequeries->size(); query_num ++){
        int64_t low = rangequeries->at(query_num).at(0);
        int64_t high = rangequeries->at(query_num).at(1);
        int64_t col = rangequeries->at(query_num).at(2);
        scan_maps(&table->crackermaps.at(leading_col).at(col), bitmap, offsets->at(0).first,offsets->at(0).second, low, high);
    }
    for(size_t i = 0; i < offsets->at(0).second - offsets->at(0).first + 1; ++i)
        if(bitmap[i])
            result->push_back(table->crackermaps.at(leading_col).at(0).ids.at(i+ offsets->at(0).first));
}
