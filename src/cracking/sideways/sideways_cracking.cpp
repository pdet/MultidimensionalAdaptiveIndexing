#include "sideways_cracking.h"

using namespace std;

extern int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS;

static vector<CrackerSets> crackersets;

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
    if(x1 > 0)
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
        pivot_pair = nullptr;
    }

    return T;
}

void crack_until_latest_query(CrackerSets &set, int64_t aux_col){

    int64_t current_position = set.crackermaps.at(aux_col).crack_position;
    
    for(int64_t i = current_position; i < set.rangequeries.size(); i++, set.crackermaps.at(aux_col).crack_position++)
    {
        int64_t low = set.rangequeries.at(i).at(0);
        int64_t high = set.rangequeries.at(i).at(1);

        set.T.at(aux_col) = sideways_cracking(&set.crackermaps.at(aux_col), set.T.at(aux_col), low, high);
    }
}

void sideways_cracking_pre_processing(Table *table, Tree * T){
    crackersets.resize(NUMBER_OF_COLUMNS);
    for (int i = 0; i < NUMBER_OF_COLUMNS ; ++i)
    {
        crackersets.at(i).leading_column = i;
        crackersets.at(i).crackermaps.resize(NUMBER_OF_COLUMNS);
        crackersets.at(i).T.resize(NUMBER_OF_COLUMNS, nullptr);
        for(size_t j = 0; j < NUMBER_OF_COLUMNS; ++j)
        {
            CrackerMaps map;
            map.leading_column = i;
            map.aux_column = j;
            map.ids=table->ids;
            map.columns.push_back(table->columns.at(i));  // Always get most selective columns as leading column
            map.columns.push_back(table->columns.at(j));
            map.crack_position = 0;
            crackersets.at(i).crackermaps.at(j) = map;
        }
    }
}

void sideways_cracking_partial_built(Table *table, Tree * T, vector<array<int64_t, 3>>  *rangequeries){
    int64_t leading_col = rangequeries->at(0).at(2);

    crackersets.at(leading_col).rangequeries.push_back(rangequeries->at(0));
    for(size_t i = 0; i < rangequeries->size(); i ++){
        int64_t c = rangequeries->at(i).at(2);
        crack_until_latest_query(crackersets.at(leading_col), c);
    }
}

void sideways_cracking_index_lookup(Tree * T, vector<array<int64_t, 3> >  *rangequeries,vector<pair<int,int>>  *offsets){
    int64_t low = rangequeries->at(0).at(0);
    int64_t high = rangequeries->at(0).at(1);
    int64_t col = rangequeries->at(0).at(2);
    IntPair p1, p2;
    if(low != -1)
        p1 = FindNeighborsGTE(low, crackersets.at(col).T.at(col), COLUMN_SIZE - 1);
    else
        p1->first = 0;
    
    if(high != -1)
        p2 = FindNeighborsLT(high, crackersets.at(col).T.at(col), COLUMN_SIZE - 1);
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
    vector<bool> bitmap(offsets->at(0).second - offsets->at(0).first + 1, 1);
//    Since the first predicate is used as the leading column, there is no need to re-search it
    for (size_t query_num = 1; query_num < rangequeries->size(); query_num ++){
        int64_t low = rangequeries->at(query_num).at(0);
        int64_t high = rangequeries->at(query_num).at(1);
        int64_t col = rangequeries->at(query_num).at(2);
        scan_maps(&crackersets.at(leading_col).crackermaps.at(col), bitmap, offsets->at(0).first,offsets->at(0).second, low, high);
    }
    for(size_t i = 0; i < offsets->at(0).second - offsets->at(0).first + 1; ++i)
        if(bitmap[i])
            result->push_back(crackersets.at(leading_col).crackermaps.at(leading_col).ids.at(i+ offsets->at(0).first));
}
