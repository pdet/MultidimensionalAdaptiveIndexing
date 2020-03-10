#include "column.hpp"
#include <vector>

Column::Column(std::vector<float> column_to_copy){
    data = column_to_copy;
}

Column::Column(){}

Column::Column(const Column& other){
    data = other.data;
}

void Column::append(float value){
    data.push_back(value);
}
