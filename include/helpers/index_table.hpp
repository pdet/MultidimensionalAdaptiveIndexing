#pragma once

#include "table.hpp"

class IdxCol {
public:
  IdxCol(size_t size);
  IdxCol(Column *column_to_copy);
  IdxCol() : size(0), capacity(0), data(nullptr){};
  ~IdxCol() { free(data); }
  size_t size;
  size_t capacity;
  float * data;
};

class IdxTbl {
private:
  int64_t number_of_rows;
  int64_t number_of_columns;
public:
  vector<unique_ptr<IdxCol>> columns;

  //! this is used when the first query fully copies the data (e.g., Quasii, Cracking KD-Tree)
  IdxTbl(Table *table_to_copy);
  //! This just allocates the memory, no copy is done (e.g., PI)
  IdxTbl(size_t number_of_columns, size_t number_of_rows);

  int64_t row_count() const;

  int64_t col_count() const;

  //! Cracking Methods

  //! Cracks table from position i = low until i == high
  //! on column (c) with key (element)
  //! Returns in which position the list is greater or equal to key
  //! In case of repetition, returns the first one, and all the others come
  //! right after it
  int64_t CrackTable(int64_t low, int64_t high, float element, int64_t c);

  //! Cracks table in three from position i = low until i == high
  //! on column (c) with left key and right key
  //! Returns a pair of positions indicating where each patition starts
  pair<int64_t, int64_t> CrackTableInThree(int64_t low, int64_t high, float key_left, float key_right, int64_t c);

  //! Swap elements
  void exchange(int64_t index1, int64_t index2);


};