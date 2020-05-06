#ifndef BITVECTOR_H 
#define BITVECTOR_H 

#include <cstdint>
#include <memory>
#include <stddef.h>
#include <math.h> // ceil
#include <cassert> // assert
#include <string>

class BitVector
{
private:
    size_t number_of_bits;
    std::unique_ptr<char[]> bits;
public:
    BitVector();
    BitVector(size_t number_of_bits);
    BitVector(size_t number_of_bits, bool default_value);
    BitVector(const BitVector& other) = delete; // copy constructor
    BitVector& operator=(const BitVector& other) = delete; // copy assign
    BitVector(BitVector&& other); // move constructor
    BitVector& operator=(BitVector&& other); // move assign
    ~BitVector();

    void set(size_t pos, bool value);
    bool get(size_t pos) const;
    void bitwise_and(const BitVector& bitvector);
    size_t size() const;

    std::string to_string()
    {
        std::string str;
        for(size_t i = 0; i < size(); ++i){
            if(get(i) == true){
                str+= '1';
            }else{
                str+='0';
            }
        }
        return str; 
    }
};

inline BitVector::BitVector() : number_of_bits(0), bits(nullptr){
}

inline BitVector::BitVector(size_t number_of_bits) : number_of_bits(number_of_bits){
    // need to watch out for integer division
    bits = std::unique_ptr<char[]> {new char[number_of_bits]}; 
    for(size_t i = 0; i < number_of_bits; ++i){
        bits[i] = false;
    }

}

inline BitVector::BitVector(size_t number_of_bits, bool default_value)
: number_of_bits(number_of_bits){
    // need to watch out for integer division
    bits = std::unique_ptr<char[]> {new char[number_of_bits]}; 
    // if default value is true set all values to true 
    if(default_value == true){
        for(size_t i = 0; i < number_of_bits; ++i){
            bits[i] = true;
        }
    }else{
        for(size_t i = 0; i < number_of_bits; ++i){
            bits[i] = false;
        }
    }
}

inline BitVector::~BitVector(){
    number_of_bits = 0;
}

inline BitVector::BitVector(BitVector&& other)
: number_of_bits(other.number_of_bits), bits(std::move(other.bits))
{
    other.number_of_bits = 0;
}

inline BitVector& BitVector::operator=(BitVector&& other)
{
    number_of_bits = other.number_of_bits;
    bits = std::move(other.bits);

    other.number_of_bits = 0;
    return *this;
}

inline void BitVector::set(size_t pos, bool value){
    assert(number_of_bits > pos);
    bits[pos] = value;
}

inline void BitVector::bitwise_and(const BitVector& bitvector){
    assert(bitvector.size() == size());
    for(size_t i = 0; i < number_of_bits; ++i){
        bits[i] &= bitvector.get(i);
    }
}
inline bool BitVector::get(size_t pos) const{
    assert(pos < number_of_bits);
    return bits[pos];
}

inline size_t BitVector::size() const{
    return number_of_bits;
}
#endif // BITVECTOR_H 
