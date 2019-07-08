#ifndef PREDICATE
#define PREDICATE

#include <cstdint>

class Predicate
{
public:
    float low;
    float high;
    float column;

    Predicate(float low, float high, float column)
        : low(low), high(high), column(column){}
    ~Predicate(){}
};

#endif