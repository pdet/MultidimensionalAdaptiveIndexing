#include "index_table.hpp"
IdxCol::IdxCol(size_t size) : size(size), capacity(size){
  data = (float*)malloc(sizeof(float) * this->size);
}

IdxCol::IdxCol(Column *column_to_copy):size(column_to_copy->data.size()),capacity(column_to_copy->data.size()){
  data = (float*)malloc(sizeof(float) * column_to_copy->data.size());
  for (size_t i = 0; i < column_to_copy->data.size(); i ++){
    data[i] = column_to_copy->data[i];
  }
}


IdxTbl::IdxTbl(Table *table_to_copy):number_of_rows(table_to_copy->row_count())
,number_of_columns(table_to_copy->col_count()) {
  columns.resize(number_of_columns);
  for (int64_t i = 0; i < number_of_columns; ++i) {
    columns[i] = make_unique<IdxCol>(table_to_copy->columns[i].get());
  }
}

int64_t IdxTbl::row_count() const{
  return number_of_rows;
}

int64_t IdxTbl::col_count() const{
  return number_of_columns;
}

int64_t IdxTbl::CrackTable(int64_t low, int64_t high, float element, int64_t c)
{
  int64_t x1 = low;
  int64_t x2 = high - 1;

  while (x1 <= x2 && x2 > 0)
  {
    if (columns[c]->data[x1] < element)
      x1++;
    else
    {
      while (x2 > 0 && x2 >= x1 && (columns[c]->data[x2] >= element))
        x2--;
      if (x1 < x2)
      {
        exchange(x1, x2);
        x1++;
        x2--;
      }
    }
  }
  return x1;
}

pair<int64_t, int64_t> IdxTbl::CrackTableInThree(int64_t low, int64_t high, float key_left, float key_right, int64_t c)
{
  auto p1 = CrackTable(low, high, key_left, c);
  auto p2 = CrackTable(p1, high, key_right, c);
  return make_pair(p1, p2);
}

void IdxTbl::exchange(int64_t index1, int64_t index2){
  for(int64_t column_index = 0; column_index < number_of_columns; ++column_index){
    auto value1 = columns[column_index]->data[index1];
    auto value2 = columns[column_index]->data[index2];

    auto tmp = value1;
    columns[column_index]->data[index1] = value2;
    columns[column_index]->data[index2] = tmp;
  }
}