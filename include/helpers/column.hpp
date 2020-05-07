#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <cstdint>

class Column{
public:
    float* data;
    size_t size;
    size_t capacity;
    Column(float* column_to_copy, size_t column_size);

    Column(size_t capacity);

    Column();

    ~Column();

    void append(float value);

    void maybe_append(float value, int maybe);
};

#endif // COLUMN_H
