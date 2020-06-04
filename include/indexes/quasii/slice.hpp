#ifndef SLICE_H
#define SLICE_H

#include <cstddef>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

// How the Slice looks like:
//         |============================|
//         | Values go from:            |
//         |    left_value (>=)         |
//         |    right_value (<)         |
//         | Column: X1                 |
//         |============================|
// Table Start Pos (>=)           Table End Pos (<)

class Slice{
public:
    size_t column;
    size_t offset_begin;
    size_t offset_end;
    float left_value;
    float right_value;
    vector<Slice> children;

    Slice(size_t column, size_t offset_begin, size_t offset_end, float left_value, float right_value);

    // "Open" slice, covers the entire range
    Slice(size_t column, size_t offset_begin, size_t offset_end);

	//Slice(const Slice&& other);

	//Slice& operator=(const Slice&& other);

    Slice();

	//Slice(const Slice& a);

	//Slice& operator=(const Slice& a); 

    string label();

    bool equal(const Slice &other);

    bool intersects(float low, float high);

    size_t size();

};

#endif // SLICE_H
