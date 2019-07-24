#ifndef SLICE_H
#define SLICE_H

#include <cstddef>
#include <vector>

using namespace std;

class Slice{
public:
    size_t column;
    size_t offset_begin;
    size_t offset_end;
    float left_value;
    float right_value;
    vector<Slice> children;

    Slice(size_t column, size_t offset_begin, size_t offset_end, float left_value, float right_value);

    Slice(const Slice &other);

    // "Open" slice, covers the entire range
    Slice(size_t column, size_t offset_begin, size_t offset_end);

    Slice();
    ~Slice();

    bool equal(const Slice &other);

    bool intersects(float low, float high);

    size_t size();

};

#endif // SLICE_H