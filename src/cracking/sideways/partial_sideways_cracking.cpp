// #include "partial_sideways_cracking.h"

// using namespace std;

// extern int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS;

// void exchange(vector<int64_t> * leading_column,vector<int64_t>*  aux_column, int64_t x1, int64_t x2)
// {
//     if (x1 == x2)
//         return;
//     int64_t tmp;
//     tmp = leading_column->at(x1);
//     leading_column->at(x1) = leading_column->at(x2);
//     leading_column->at(x2) = tmp;

//     tmp = aux_column->at(x1);
//     aux_column->at(x1) = aux_column->at(x2);
//     aux_column->at(x2) = tmp;
// }


// int64_t crack_map(vector<int64_t> * leading_column,vector<int64_t>*  aux_column, int64_t low, int64_t high, int64_t element)
// {
//     int64_t x1 = low;
//     int64_t x2 = high;

//     while (x1 <= x2)
//     {
//         if (leading_column->at(x1) < element)
//             x1++;
//         else
//         {
//             while (x2 >= x1 && (leading_column->at(x2) >= element))
//                 x2--;
//             if (x1 < x2)
//             {
//                 exchange(leading_column,aux_column, x1, x2);
//                 x1++;
//                 x2--;
//             }
//         }
//     }
//     if (x1 < x2)
//         printf("Not all elements were inspected!");
//     x1--;
//     return x1;
// }

// Tree partial_sideways_cracking(vector<int64_t> * leading_column,vector<int64_t>*  aux_column, Tree T, int lowKey, int highKey)
// {
//     IntPair p1, p2;
//     p1 = FindNeighborsLT(lowKey, T, COLUMN_SIZE - 1);
//     p2 = FindNeighborsLT(highKey, T, COLUMN_SIZE - 1);


//     IntPair pivot_pair = NULL;
  
//     // crack in two
//     pivot_pair = (IntPair)malloc(sizeof(struct int_pair));
//     pivot_pair->first = crack_map(leading_column,aux_column, p1->first, p1->second, lowKey);
//     pivot_pair->second = crack_map(leading_column,aux_column, pivot_pair->first, p2->second, highKey);

//     T = Insert(pivot_pair->first, lowKey, T);
//     T = Insert(pivot_pair->second, highKey, T);

//     free(p1);
//     free(p2);
//     if (pivot_pair)
//     {
//         free(pivot_pair);
//         pivot_pair = NULL;
//     }

//     return T;
// }

// void partial_sideways_cracking_pre_processing(Table *table, Tree * T){
//     table->mapset.chunkmap.ids = table->ids;
//     table->mapset.chunkmap.leading_column = table->columns.at(0);
//     PartialMaps pmap;
//     pmap.leading_column = vector<int64_t> (COLUMN_SIZE);
//     pmap.aux_column = vector<int64_t> (COLUMN_SIZE);
//     pmap.T = NULL;
//      for (size_t j = 1; j < NUMBER_OF_COLUMNS ; ++j)
//     {
//         table->mapset.partialMaps.push_back(pmap);
//     }
//     *T = NULL;
// }



// void partial_sideways_cracking_partial_built(Table *table, Tree * T,vector<pair<int64_t,int64_t>>  *rangequeries){
//     if (!*T){
//         *T = partial_sideways_cracking(&table->mapset.chunkmap.leading_column,&table->mapset.chunkmap.ids,*T,rangequeries->at(0).first,rangequeries->at(0).second);
//         IntPair p1 = FindNeighborsGTE(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
//         IntPair p2 = FindNeighborsLT(rangequeries->at(0).second, *T, COLUMN_SIZE - 1);
//         for (size_t i = 0; i < table->mapset.partialMaps.size(); ++i){
//             for (size_t j = p1->first; j < p2->second; ++j){
//                 table->mapset.partialMaps.at(i).leading_column.at(j) = table->mapset.chunkmap.leading_column.at(j);
//                 int64_t id = table->mapset.chunkmap.ids.at(j);
//                 table->mapset.partialMaps.at(i).aux_column.at(j) = table->columns.at(i+1).at(id);
//             }
//             table->mapset.partialMaps.at(i).T = *T;                
//         }
//     }

//     else{
//         IntPair p1 = FindNeighborsGTE(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
//         IntPair p2 = FindNeighborsLT(rangequeries->at(0).second, *T, COLUMN_SIZE - 1);

//         if (p1->first == 0 && p2 ->second == COLUMN_SIZE-1){
//             *T = partial_sideways_cracking(&table->mapset.chunkmap.leading_column,&table->mapset.chunkmap.ids,*T,rangequeries->at(0).first,rangequeries->at(0).second);
//             IntPair lq = FindNeighborsGTE(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
//             IntPair hq = FindNeighborsGTE(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
//             for (size_t i = 0; i < table->mapset.partialMaps.size(); ++i){
//                 for (size_t j = lq->first; j < lq->second; ++j){
//                     table->mapset.partialMaps.at(i).leading_column.at(j) = table->mapset.chunkmap.leading_column.at(j);
//                     int64_t id = table->mapset.chunkmap.ids.at(j);
//                     table->mapset.partialMaps.at(i).aux_column.at(j) = table->columns.at(i+1).at(id);              
//                 }
//                 table->mapset.partialMaps.at(i).T = *T;
//                 for (size_t j = hq->first; j < hq->second; ++j){
//                     table->mapset.partialMaps.at(i).leading_column.at(j) = table->mapset.chunkmap.leading_column.at(j);
//                     int64_t id = table->mapset.chunkmap.ids.at(j);
//                     table->mapset.partialMaps.at(i).aux_column.at(j) = table->columns.at(i+1).at(id);
//                     table->mapset.partialMaps.at(i).T = *T;                
//                 }
//             }
//         }

//         else if (p1->first == 0){
//             int64_t pos = crack_map(&table->mapset.chunkmap.leading_column,&table->mapset.chunkmap.ids, p1->first, p1->second, rangequeries->at(0).first);
//             *T = Insert(pos, rangequeries->at(0).first, *T);
//             IntPair lq = FindNeighborsGTE(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
//             for (size_t i = 0; i < table->mapset.partialMaps.size(); ++i){
//                 for (size_t j = lq->first; j < lq->second; ++j){
//                     table->mapset.partialMaps.at(i).leading_column.at(j) = table->mapset.chunkmap.leading_column.at(j);
//                     int64_t id = table->mapset.chunkmap.ids.at(j);
//                     table->mapset.partialMaps.at(i).aux_column.at(j) = table->columns.at(i+1).at(id);
//                 }
//                 table->mapset.partialMaps.at(i).T = Insert(pos, rangequeries->at(0).first, table->mapset.partialMaps.at(i).T );
//                 pos = crack_map(&table->mapset.partialMaps.at(i).leading_column,&table->mapset.partialMaps.at(i).aux_column, p2->first, p2->second, rangequeries->at(0).second);
//                 table->mapset.partialMaps.at(i).T = Insert(pos, rangequeries->at(0).second, table->mapset.partialMaps.at(i).T );
//             }

//         }

//         else if (p2 ->second == COLUMN_SIZE-1){
//             int64_t pos = crack_map(&table->mapset.chunkmap.leading_column,&table->mapset.chunkmap.ids, p2->first, p2->second, rangequeries->at(0).second);

//             *T = Insert(pos, rangequeries->at(0).second, *T);
//             IntPair hq = FindNeighborsLT(rangequeries->at(0).second, *T, COLUMN_SIZE - 1);

//             for (size_t i = 0; i < table->mapset.partialMaps.size(); ++i){
//                 for (size_t j = hq->first; j < hq->second; ++j){
//                     table->mapset.partialMaps.at(i).leading_column.at(j) = table->mapset.chunkmap.leading_column.at(j);
//                     int64_t id = table->mapset.chunkmap.ids.at(j);
//                     table->mapset.partialMaps.at(i).aux_column.at(j) = table->columns.at(i+1).at(id);
//                 }

//                 p1 = FindNeighborsLT(rangequeries->at(0).first, *T, COLUMN_SIZE - 1);
//                 table->mapset.partialMaps.at(i).T = Insert(pos, rangequeries->at(0).second, table->mapset.partialMaps.at(i).T );
//                 pos = crack_map(&table->mapset.partialMaps.at(i).leading_column,&table->mapset.partialMaps.at(i).aux_column, p1->first,p1->second, rangequeries->at(0).first);
//                 table->mapset.partialMaps.at(i).T = Insert(pos, rangequeries->at(0).first, table->mapset.partialMaps.at(i).T);
//             }
//         }

//         else{

//                 p1 = FindNeighborsLT(rangequeries->at(0).first, table->mapset.partialMaps.at(0).T, COLUMN_SIZE - 1);
//                 p2 = FindNeighborsLT(rangequeries->at(0).second, table->mapset.partialMaps.at(0).T, COLUMN_SIZE - 1);
//             for (size_t i = 0; i < table->mapset.partialMaps.size(); ++i){


//                 IntPair pivot_pair = NULL;
              
//                 // crack in two
//                 pivot_pair = (IntPair)malloc(sizeof(struct int_pair));
//                 pivot_pair->first = crack_map(&table->mapset.partialMaps.at(i).leading_column,&table->mapset.partialMaps.at(i).aux_column, p1->first, p1->second, rangequeries->at(0).first);
//                 pivot_pair->second = crack_map(&table->mapset.partialMaps.at(i).leading_column,&table->mapset.partialMaps.at(i).aux_column, pivot_pair->first, p2->second, rangequeries->at(0).second);

//                 table->mapset.partialMaps.at(i).T = Insert(pivot_pair->first, rangequeries->at(0).first, table->mapset.partialMaps.at(i).T);
//                 table->mapset.partialMaps.at(i).T = Insert(pivot_pair->second, rangequeries->at(0).second, table->mapset.partialMaps.at(i).T);
//             }

//         }
//     }

// }


// void scan_maps(vector<int64_t>*  aux_column, boost::dynamic_bitset<> &bitmap, int lowOffset, int highOffset, int lowKey, int highKey){
//     for(boost::dynamic_bitset<>::size_type i = 0; i < highOffset - lowOffset; ++i)
//         if(bitmap[i])
//             if(aux_column->at(lowOffset+i) < lowKey || aux_column->at(lowOffset+i) >= highKey)
//                 bitmap[i] = 0;
// }


// void partial_sideways_cracking_scan(Table *table, vector<pair<int64_t,int64_t>>  *rangequeries,vector<pair<int,int>> *offsets, vector<int64_t> * result){
//     IntPair p1 = FindNeighborsGTE(rangequeries->at(0).first, table->mapset.partialMaps.at(0).T, COLUMN_SIZE - 1);
//     IntPair p2 = FindNeighborsLT(rangequeries->at(0).second, table->mapset.partialMaps.at(0).T, COLUMN_SIZE - 1);
//     offsets->push_back(make_pair(p1->first, p2->second));
//     boost::dynamic_bitset<> bitmap(offsets->at(0).second - offsets->at(0).first); 
//     for(boost::dynamic_bitset<>::size_type i = 0; i < offsets->at(0).second - offsets->at(0).first; ++i)
//         bitmap[i]=1;
//     for (size_t i = 0; i < table->mapset.partialMaps.size(); i ++){
//         scan_maps(&table->mapset.partialMaps.at(i).aux_column,bitmap,offsets->at(0).first,offsets->at(0).second,rangequeries->at(1+i).first,rangequeries->at(1+i).second);
//     }

//     #ifndef test
//     result->push_back(0);
//     #endif
//      #ifdef test
//          for(boost::dynamic_bitset<>::size_type i = 0; i < offsets->at(0).second - offsets->at(0).first; ++i)
//             if(bitmap[i])
//                 result->push_back(table->mapset.partialMaps.at(0).leading_column.at(i+ offsets->at(0).first));
//             #else
//             for(boost::dynamic_bitset<>::size_type i = 0; i < offsets->at(0).second - offsets->at(0).first; ++i)
//                 if(bitmap[i])
//                     result->at(0)+=1;
//             #endif  

// }


