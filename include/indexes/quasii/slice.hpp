#ifndef SLICE_H
#define SLICE_H

#include <cstddef>
#include <vector>
#include <cstdint>

using namespace std;

class Slice{
public:
    int64_t column;
    int64_t offset_begin;
    int64_t offset_end;
    float left_value;
    float right_value;
    vector<Slice> children;

    Slice(int64_t column, int64_t offset_begin, int64_t offset_end, float left_value, float right_value);

    Slice(const Slice &other);

    // "Open" slice, covers the entire range
    Slice(int64_t column, int64_t offset_begin, int64_t offset_end);

    Slice();
    ~Slice();

    bool equal(const Slice &other);

    bool intersects(float low, float high);

    int64_t size();

};

#endif // SLICE_H
