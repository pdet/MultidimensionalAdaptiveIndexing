#ifndef PREDICATE_H
#define PREDICATE_H

#include <cstdint>
#include <cstddef>

class Predicate
{
public:
    float low;
    float high;
    int64_t column;

    Predicate(float low, float high, int64_t column);
    Predicate();
    ~Predicate();
};

#endif // PREDICATE_H
