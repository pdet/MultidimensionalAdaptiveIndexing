#ifndef PREDICATE
#define PREDICATE

#include <cstdint>

class Predicate
{
public:
    int64_t low;
    int64_t high;
    int64_t column;

    Predicate(int64_t low, int64_t high, int64_t column)
        : low(low), high(high), column(column){}
    ~Predicate(){}
};

#endif