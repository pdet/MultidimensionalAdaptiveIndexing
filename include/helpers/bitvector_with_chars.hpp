#ifndef BITVECTOR_WITH_CHARS_H 
#define BITVECTOR_WITH_CHARS_H 

#include <cstdint>
#include <memory>
#include <stddef.h>
#include <math.h> // ceil
#include <cassert> // assert
#include <string>

class BitVectorWithChars
{
private:
    size_t number_of_bits;
    std::unique_ptr<char[]> bits;
public:
    BitVectorWithChars();
    BitVectorWithChars(size_t number_of_bits);
    BitVectorWithChars(size_t number_of_bits, bool default_value);
    BitVectorWithChars(const BitVectorWithChars& other) = delete; // copy constructor
    BitVectorWithChars& operator=(const BitVectorWithChars& other) = delete; // copy assign
    BitVectorWithChars(BitVectorWithChars&& other); // move constructor
    BitVectorWithChars& operator=(BitVectorWithChars&& other); // move assign
    ~BitVectorWithChars();

    void set(size_t pos, bool value);
    bool get(size_t pos) const;
    void bitwise_and(const BitVectorWithChars& bitvector);
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

inline BitVectorWithChars::BitVectorWithChars() : number_of_bits(0), bits(nullptr){
}

inline BitVectorWithChars::BitVectorWithChars(size_t number_of_bits) : number_of_bits(number_of_bits){
    // need to watch out for integer division
    bits = std::unique_ptr<char[]> {new char[number_of_bits]}; 
    for(size_t i = 0; i < number_of_bits; ++i){
        bits[i] = false;
    }
}

inline BitVectorWithChars::BitVectorWithChars(size_t number_of_bits, bool default_value)
: number_of_bits(number_of_bits){
    // need to watch out for integer division
    bits = std::unique_ptr<char[]> {new char[number_of_bits]}; 
    // if default value is true set all values to true 
    for (size_t i = 0; i < number_of_bits; ++i) {
        bits[i] = default_value;
    }
}

inline BitVectorWithChars::~BitVectorWithChars(){
    number_of_bits = 0;
}

inline BitVectorWithChars::BitVectorWithChars(BitVectorWithChars&& other)
: number_of_bits(other.number_of_bits), bits(std::move(other.bits))
{
    other.number_of_bits = 0;
}

inline BitVectorWithChars& BitVectorWithChars::operator=(BitVectorWithChars&& other)
{
    number_of_bits = other.number_of_bits;
    bits = std::move(other.bits);

    other.number_of_bits = 0;
    return *this;
}

inline void BitVectorWithChars::set(size_t pos, bool value){
    assert(number_of_bits > pos);
    bits[pos] = value;
}

inline void BitVectorWithChars::bitwise_and(const BitVectorWithChars& bitvector){
    assert(bitvector.size() == size());
    for(size_t i = 0; i < number_of_bits; ++i){
        bits[i] &= bitvector.get(i);
    }
}
inline bool BitVectorWithChars::get(size_t pos) const{
    assert(pos < number_of_bits);
    return bits[pos];
}

inline size_t BitVectorWithChars::size() const{
    return number_of_bits;
}
#endif // BITVECTOR_WITH_CHARS_H 
