#ifndef PREDICATE
#define PREDICATE

#include <cstdint>
#include <cstddef>

class Predicate
{
public:
    float low;
    float high;
    size_t column;

    Predicate(float low, float high, size_t column)
        : low(low), high(high), column(column){}
    ~Predicate(){}
};

#endif