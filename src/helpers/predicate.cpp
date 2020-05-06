#include "predicate.hpp"
#include <cstdint>
#include <cstddef>

Predicate::Predicate(float low, float high, size_t column)
    : low(low), high(high), column(column){}

Predicate::Predicate() : low(0), high(0), column(0){}

Predicate::~Predicate(){}

