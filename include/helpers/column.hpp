#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <cstdint>

class Column{
public:
    std::vector<float> data;
    Column(std::vector<float> column_to_copy);

    Column();

    Column(const Column &other);

    void append(float value);
};

#endif // COLUMN_H
