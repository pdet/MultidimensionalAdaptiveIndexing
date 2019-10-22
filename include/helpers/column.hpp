#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <cstdint>

class Column{
private:
    std::vector<float> data;

public:
    Column(std::vector<float> column_to_copy);

    Column();

    Column(const Column &other);

    void append(float value);

    float at(int64_t index);

    void assign(int64_t index, float value);
};

#endif // COLUMN_H
