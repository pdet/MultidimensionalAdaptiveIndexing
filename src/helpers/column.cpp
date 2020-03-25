#include "column.hpp"
#include <vector>

Column::Column(float* column_to_copy, size_t column_size){
    data = (float*)malloc(sizeof(float) * column_size);
    for (size_t i = 0; i < column_size; i ++){
        data[i] = column_to_copy[i];
    }
    this->size = column_size;
    this->capacity = column_size;
}

Column::Column(size_t size) : size(size), capacity(size) {
    data = (float*)malloc(sizeof(float) * this->capacity);
}

Column::Column() : size(0), capacity(1){
    data = (float*) malloc(sizeof(float) * this->capacity);
}

Column::~Column(){
    free(data);
}

void Column::append(float value){
    if(size == capacity){
        this->capacity *= 2;
        data = (float *) realloc(data, sizeof(float) * this->capacity);
    }
    data[size] = value;
    size++;
}
