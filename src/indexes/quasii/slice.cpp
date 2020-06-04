#include "slice.hpp"
#include <limits>

Slice::Slice(size_t c, size_t offset_begin, size_t offset_end, float left_value, float right_value)
: column(c), offset_begin(offset_begin), offset_end(offset_end), left_value(left_value), right_value(right_value){
    children = vector<Slice>();
}

//Slice::Slice(const Slice &&other){
//    column = other.column;
//    offset_begin = other.offset_begin;
//    offset_end = other.offset_end;
//    left_value = other.left_value;
//    right_value = other.right_value;
//    children = std::move(other.children);
//}
//
//Slice& Slice::operator=(const Slice&& other){
//    column = other.column;
//    offset_begin = other.offset_begin;
//    offset_end = other.offset_end;
//    left_value = other.left_value;
//    right_value = other.right_value;
//    children = std::move(other.children);
//    return *this;
//}   
//
//Slice::Slice(const Slice &other){
//    column = other.column;
//    offset_begin = other.offset_begin;
//    offset_end = other.offset_end;
//    left_value = other.left_value;
//    right_value = other.right_value;
//    children = other.children;
//}
//
//Slice& Slice::operator=(const Slice& other){
//    column = other.column;
//    offset_begin = other.offset_begin;
//    offset_end = other.offset_end;
//    left_value = other.left_value;
//    right_value = other.right_value;
//    children = other.children;
//    return *this;
//}

// "Open" slice, covers the entire range
Slice::Slice(size_t column, size_t offset_begin, size_t offset_end)
    : column(column)
    , offset_begin(offset_begin)
    , offset_end(offset_end)
    , left_value(numeric_limits<float>::lowest())
    , right_value(numeric_limits<float>::max()){}

Slice::Slice(){
    column = 0;
    offset_begin = 0;
    offset_end = 0;
    left_value = 0;
    right_value = 0;
}

string Slice::label(){
    auto offsets = std::to_string(offset_begin) + '-' + std::to_string(offset_end);
    auto pivots = std::to_string(left_value) + '-' + std::to_string(right_value);
    return "Column: " + std::to_string(column) + "\\n" +\
           "Offsets: " + offsets + "\\n" +\
           "Pivots: " + pivots;
}

bool Slice::equal(const Slice &other){
    return (
        column == other.column &&
        offset_begin == other.offset_begin &&
        offset_end == other.offset_end &&
        left_value == other.left_value &&
        right_value == other.right_value
    );
}

bool Slice::intersects(float low, float high){
    return (
        (left_value <= low && low < right_value) ||
        (left_value <= high && high <= right_value) ||
        (low <= left_value && right_value <= high)
    );
}

size_t Slice::size(){
    return offset_end - offset_begin;
}
