#include <catch.hpp>
#include "bitvector.hpp"

TEST_CASE( "Test set", "[BitVector]")
{
    BitVector bv(1000, false);
    for(size_t i = 0; i < bv.size(); ++i)
        bv.set(i);

    for(size_t i = 0; i < bv.size(); ++i)
        REQUIRE(bv.get(i) == true);
}

TEST_CASE( "Test unset", "[BitVector]")
{
    BitVector bv(1000, true);
    for(size_t i = 0; i < bv.size(); ++i)
        bv.unset(i);

    for(size_t i = 0; i < bv.size(); ++i)
        REQUIRE(bv.get(i) == false);
}

TEST_CASE( "Test default value equal true", "[BitVector]")
{
    BitVector bv(1000, true);

    for(size_t i = 0; i < bv.size(); ++i)
        REQUIRE(bv.get(i) == true);
}

TEST_CASE( "Test default value equal false", "[BitVector]")
{
    BitVector bv(1000, false);

    for(size_t i = 0; i < bv.size(); ++i)
        REQUIRE(bv.get(i) == false);
}

TEST_CASE( "Test bitwise and", "[BitVector]")
{
    BitVector bv1(1000, true);
    for(size_t i = 0; i < bv1.size(); ++i)
        bv1.set(i);
    BitVector bv2(1000);
    for(size_t i = 0; i < bv2.size(); i += 2)
        bv2.set(i);

    bv1.bitwise_and(bv2);

    for(size_t i = 0; i < bv1.size(); ++i){
        if(i % 2 == 0){
            REQUIRE(bv1.get(i) == true);
        }else{
            REQUIRE(bv1.get(i) == false);
        }
    }
}
