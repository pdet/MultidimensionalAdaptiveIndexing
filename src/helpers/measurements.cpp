#include <chrono>
#include <vector>
#include "measurements.hpp"

Measurements::Measurements(){}
Measurements::~Measurements(){}

Measurements::time_point Measurements::time(){
    return std::chrono::steady_clock::now();
}

std::chrono::nanoseconds Measurements::difference(Measurements::time_point end, Measurements::time_point start){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}