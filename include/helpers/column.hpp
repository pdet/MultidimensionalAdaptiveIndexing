#ifndef COLUMN_H
#define COLUMN_H

#include <vector>

class Column{
private:
    std::vector<float> data;

public:
    Column(std::vector<float> column_to_copy);

    Column();

    void append(float value);

    float at(size_t index);

    void assign(size_t index, float value);
};

#endif // COLUMN_H