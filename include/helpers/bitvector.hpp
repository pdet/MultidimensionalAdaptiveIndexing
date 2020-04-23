#ifndef BITVECTOR_H 
#define BITVECTOR_H 

#include <cstdint>
#include <stddef.h>

class BitVector
{
private:
    using b_type = uint64_t;
    const size_t type_size = sizeof(b_type);
    size_t number_of_bits;
    size_t vector_size;
    b_type* bits;
public:
    BitVector();
    BitVector(size_t number_of_bits);
    BitVector(size_t number_of_bits, bool default_value);
    BitVector(const BitVector& other); // copy constructor
    BitVector(BitVector&& other); // move constructor
    BitVector& operator=(const BitVector& other); // copy assign
    BitVector& operator=(BitVector&& other); // move assign
    ~BitVector();

    void set(size_t pos);
    void unset(size_t pos);
    void bitwise_and(const BitVector& bitvector);
    bool get(size_t pos) const;
    size_t size() const;
    b_type get_raw_element(size_t pos) const;

};
#endif // BITVECTOR_H 
