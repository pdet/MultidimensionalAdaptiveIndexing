#ifndef COLUMN
#define COLUMN

#include <vector>

class Column{
private:
    std::vector<float> data;

public:
    Column(std::vector<float> column_to_copy){
        data = column_to_copy;
    }

    Column(){}

    void append(float value){
        data.push_back(value);
    }

    float at(size_t index){
        return data.at(index);
    }

    void assign(size_t index, float value){
        data.at(index) = value;
    }
};

#endif