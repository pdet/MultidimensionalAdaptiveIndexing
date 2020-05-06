#include "bitvector.hpp"
#include <math.h> // ceil
#include <cassert> // assert

BitVector::BitVector() {
    number_of_bits = 0;
    vector_size = 0;
    bits = nullptr;
}
BitVector::BitVector(size_t number_of_bits) : number_of_bits(number_of_bits){
    assert(number_of_bits >= 0);
    // need to watch out for integer division
    vector_size = ceil(number_of_bits/static_cast<float>(type_size));
    bits = new b_type[vector_size];
    // set all bits to 0 initially
    for(size_t i = 0; i < vector_size; ++i){
        bits[i] = 0;
    }
}

BitVector::BitVector(size_t number_of_bits, bool default_value)
: number_of_bits(number_of_bits){
    assert(number_of_bits >= 0);
    // need to watch out for integer division
    vector_size = ceil(number_of_bits/static_cast<float>(type_size));
    bits = new b_type[vector_size];
    // if default value is true set all values to true 
    if(default_value == true){
        for(size_t i = 0; i < vector_size; ++i){
            bits[i] = ~(static_cast<b_type>(0));
        }
    }
    else{
        for(size_t i = 0; i < vector_size; ++i){
            bits[i] = 0;
        }
    }
}

BitVector::~BitVector(){
    delete[] bits;
    vector_size = 0;
    number_of_bits = 0;
}

BitVector::BitVector(const BitVector& other)
: number_of_bits(other.number_of_bits), vector_size(other.vector_size)
{
    bits = new b_type[vector_size];
    for(size_t i = 0; i < vector_size; ++i){
        bits[i] = other.get_raw_element(i);
    }
}

BitVector& BitVector::operator=(const BitVector& other)
{
    *this = BitVector(other);
    return *this;
}

BitVector::BitVector(BitVector&& other)
: number_of_bits(other.number_of_bits), vector_size(other.vector_size), bits(other.bits)
{
    other.bits = nullptr;
    other.number_of_bits = 0;
    other.vector_size = 0;
}

BitVector& BitVector::operator=(BitVector&& other)
{
    number_of_bits = other.number_of_bits;
    vector_size = other.vector_size;
    bits = other.bits;

    other.bits = nullptr;
    other.number_of_bits = 0;
    other.vector_size = 0;
    return *this;
}
void BitVector::set(size_t pos){
    assert(number_of_bits > pos);
    bits[pos/type_size] |= 1 << (pos % type_size);
}
void BitVector::unset(size_t pos){
    assert(number_of_bits > pos);
    bits[pos/type_size] &= ~(1 << (pos % type_size));
}
void BitVector::bitwise_and(const BitVector& bitvector){
    assert(bitvector.size() == size());
    for(size_t i = 0; i < vector_size; ++i){
        bits[i] &= bitvector.get_raw_element(i);
    }
}
bool BitVector::get(size_t pos) const{
    assert(pos < number_of_bits);
    return bits[pos/type_size] & (1 << (pos % type_size));
}

BitVector::b_type BitVector::get_raw_element(size_t pos) const{
    assert(pos < vector_size);
    return bits[pos];
}

size_t BitVector::size() const{
    return number_of_bits;
}
