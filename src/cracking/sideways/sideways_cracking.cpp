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

Tree sideways_cracking(CrackerMaps *map, Tree T, int lowKey, int highKey)
{
    IntPair p1, p2;

    p1 = FindNeighborsLT(lowKey, T, COLUMN_SIZE - 1);
    p2 = FindNeighborsLT(highKey, T, COLUMN_SIZE - 1);

    IntPair pivot_pair = NULL;
  
    // crack in two
    pivot_pair = (IntPair)malloc(sizeof(struct int_pair));
    pivot_pair->first = crack_map(map, p1->first, p1->second, lowKey);
    pivot_pair->second = crack_map(map, pivot_pair->first, p2->second, highKey);

    T = Insert(pivot_pair->first, lowKey, T);
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
     for (size_t j = 1; j < NUMBER_OF_COLUMNS ; ++j)
    {
        CrackerMaps map;
        map.leading_column = 0;
        map.aux_column = j;
        map.ids=table->ids;
        map.columns.push_back(table->columns.at(0));  // Always get most selective columns as leading column
        map.columns.push_back(table->columns.at(j));  
        table->crackermaps.push_back(map);
    }
    *T = NULL;
}



void sideways_cracking_partial_built(Table *table, Tree * T,vector<pair<int64_t,int64_t>>  *rangequeries){
    for (size_t i = 1; i < table->crackermaps.size(); i ++){
        IntPair p1, p2;
        IntPair pivot_pair = NULL;

        p1 = FindNeighborsLT(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
        p2 = FindNeighborsLT(rangequeries->at(0).second, *T, COLUMN_SIZE - 1);
        pivot_pair = (IntPair)malloc(sizeof(struct int_pair));
        pivot_pair->first = crack_map(&table->crackermaps.at(i), p1->first, p1->second, rangequeries->at(0).first);
        pivot_pair->second = crack_map(&table->crackermaps.at(i), pivot_pair->first, p2->second, rangequeries->at(0).second);
    }
        *T = sideways_cracking(&table->crackermaps.at(0),*T,rangequeries->at(0).first,rangequeries->at(0).second);

}

void sideways_cracking_index_lookup(Tree * T,vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>>  *offsets){
    IntPair p1 = FindNeighborsGTE(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
    IntPair p2 = FindNeighborsLT(rangequeries->at(0).second, *T, COLUMN_SIZE - 1);
    offsets->push_back(make_pair(p1->first, p2->second));
}

void scan_maps(CrackerMaps *map, boost::dynamic_bitset<> &bitmap, int lowOffset, int highOffset, int lowKey, int highKey){
    for(boost::dynamic_bitset<>::size_type i = 0; i < highOffset - lowOffset; ++i)
        if(bitmap[i])
            if(map->columns.at(1).at(lowOffset+i) < lowKey || map->columns.at(1).at(lowOffset+i) >= highKey)
                bitmap[i] = 0;
}

void sideways_cracking_scan(Table *table, vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>> *offsets, vector<int64_t> * result){
    boost::dynamic_bitset<> bitmap(offsets->at(0).second - offsets->at(0).first); 
    for(boost::dynamic_bitset<>::size_type i = 0; i < offsets->at(0).second - offsets->at(0).first; ++i)
        bitmap[i]=1;
    for (size_t i = 0; i < table->crackermaps.size(); i ++){
        scan_maps(&table->crackermaps.at(i),bitmap,offsets->at(0).first,offsets->at(0).second,rangequeries->at(1+i).first,rangequeries->at(1+i).second);
    }
    
    #ifndef test
    result->push_back(0);
    #endif
     #ifdef test
         for(boost::dynamic_bitset<>::size_type i = 0; i < offsets->at(0).second - offsets->at(0).first; ++i)
            if(bitmap[i])
                result->push_back(table->crackermaps.at(0).ids.at(i+ offsets->at(0).first));
            #else
            for(boost::dynamic_bitset<>::size_type i = 0; i < offsets->at(0).second - offsets->at(0).first; ++i)
                if(bitmap[i])
                    result->at(0)+=1;
            #endif   
}
