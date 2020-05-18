#ifndef BITVECTOR_H 
#define BITVECTOR_H 

#include <cstdint>
#include <memory>
#include <stddef.h>
#include <math.h> // ceil
#include <cassert> // assert
#include <string>
#include <tgmath.h>

class BitVector
{
private:
    using b_type = uint64_t;
    size_t number_of_bits = 0;
    size_t vector_size = 0;
    std::unique_ptr<b_type[]> bits;
    size_t type_size = sizeof(b_type) * 8; // size in bits
    size_t log_type_size = std::log2<size_t>(type_size);
public:
    BitVector();
    BitVector(size_t number_of_bits);
    BitVector(size_t number_of_bits, bool default_value);
    BitVector(const BitVector& other) = delete; // copy constructor
    BitVector& operator=(const BitVector& other) = delete; // copy assign
    BitVector(BitVector&& other); // move constructor
    BitVector& operator=(BitVector&& other); // move assign
    ~BitVector();

    void set(size_t pos);
    void set(size_t pos, bool value);
    void unset(size_t pos);
    bool get(size_t pos) const;
    void bitwise_and(const BitVector& bitvector);
    b_type get_raw_element(size_t pos) const;
    size_t size() const;

    std::string to_string()
    {
        std::string str;
        for(size_t i = 0; i < size(); ++i){
            if(get(i)){
                str+= '1';
            }else{
                str+='0';
            }
        }
        return str; 
    }
};

inline BitVector::BitVector() {
    number_of_bits = 0;
    vector_size = 0;
    bits = nullptr;
}


inline BitVector::BitVector(size_t number_of_bits) : number_of_bits(number_of_bits){
    // need to watch out for integer division
    vector_size = ceil(number_of_bits/static_cast<float>(type_size));
    bits = std::unique_ptr<b_type[]> {new b_type[vector_size]}; 
    // set all bits to 0 initially
    for(size_t i = 0; i < vector_size; ++i){
        bits[i] = 0;
    }
}

inline BitVector::BitVector(size_t number_of_bits, bool default_value)
: number_of_bits(number_of_bits){
    // need to watch out for integer division
    vector_size = ceil(number_of_bits/static_cast<float>(type_size));
    bits = std::unique_ptr<b_type[]> {new b_type[vector_size]}; 
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

inline BitVector::~BitVector(){
    vector_size = 0;
    number_of_bits = 0;
}


inline BitVector::BitVector(BitVector&& other)
: number_of_bits(other.number_of_bits), vector_size(other.vector_size), bits(std::move(other.bits))
{
    other.bits = nullptr;
    other.number_of_bits = 0;
    other.vector_size = 0;
}


inline BitVector& BitVector::operator=(BitVector&& other)
{
    number_of_bits = other.number_of_bits;
    vector_size = other.vector_size;
    bits = std::move(other.bits);

    other.bits = nullptr;
    other.number_of_bits = 0;
    other.vector_size = 0;
    return *this;
}

inline void BitVector::set(size_t pos, bool value){
    size_t n = pos & (type_size - 1);
    bits[pos >> log_type_size] = (bits[pos >> log_type_size] & ~(static_cast<b_type>(1) << n)) | (static_cast<b_type>(value) << n);
}

inline void BitVector::set(size_t pos){
    bits[pos >> log_type_size] |= static_cast<b_type>(1) << (pos & (type_size-1));
}

inline void BitVector::unset(size_t pos){
    bits[pos >> log_type_size] &= ~(static_cast<b_type>(1) << (pos & (type_size-1)));
}


inline void BitVector::bitwise_and(const BitVector& bitvector){
    assert(bitvector.size() == size());
    for(size_t i = 0; i < vector_size; ++i){
        bits[i] &= bitvector.get_raw_element(i);
    }
}


inline bool BitVector::get(size_t pos) const{
    return bits[pos >> log_type_size] & (static_cast<b_type>(1) << (pos & (type_size-1)));
}


inline BitVector::b_type BitVector::get_raw_element(size_t pos) const{
    assert(pos < vector_size);
    return bits[pos];
}


inline size_t BitVector::size() const{
    return number_of_bits;
}
#endif // BITVECTOR_H 
