#include <catch.hpp>
#include "bitvector.hpp"

TEST_CASE( "Test set", "[BitVector]")
{
    BitVector bv(1000, false);
    for(size_t i = 0; i < bv.size(); ++i)
        bv.set(i, true);

    for(size_t i = 0; i < bv.size(); ++i)
        CHECK(bv.get(i) == true);
}

TEST_CASE( "Test unset", "[BitVector]")
{
    BitVector bv(1000, true);
    for(size_t i = 0; i < bv.size(); ++i)
        bv.set(i, false);

    for(size_t i = 0; i < bv.size(); ++i){
        INFO(i);
        CHECK(bv.get(i) == false);
    }
}

TEST_CASE( "Test to_string", "[BitVector]")
{
    BitVector bv(10);
    for(size_t i = 0; i < bv.size(); i += 2)
        bv.set(i, true);

    std::string expected = "1010101010";
    CHECK(bv.to_string() == expected);
}

TEST_CASE( "Test default value equal true", "[BitVector]")
{
    BitVector bv(1000, true);

    for(size_t i = 0; i < bv.size(); ++i)
        CHECK(bv.get(i) == true);
}

TEST_CASE( "Test default value equal false", "[BitVector]")
{
    BitVector bv(1000, false);

    for(size_t i = 0; i < bv.size(); ++i)
        CHECK(bv.get(i) == false);
}

TEST_CASE( "Test bitwise and", "[BitVector]")
{
    BitVector bv1(1000, true);
    for(size_t i = 0; i < bv1.size(); ++i)
        bv1.set(i, true);
    BitVector bv2(1000);
    for(size_t i = 0; i < bv2.size(); i += 2)
        bv2.set(i, true);

    bv1.bitwise_and(bv2);

    for(size_t i = 0; i < bv1.size(); ++i){
        INFO(i);
        if(i % 2 == 0){
            CHECK(bv1.get(i) == true);
        }else{
            CHECK(bv1.get(i) == false);
        }
    }
}
