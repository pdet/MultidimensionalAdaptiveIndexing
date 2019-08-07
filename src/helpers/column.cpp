#include <vector>
#include "column.hpp"

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

float Column::at(size_t index){
    return data.at(index);
}

void Column::assign(size_t index, float value){
    data.at(index) = value;
}
