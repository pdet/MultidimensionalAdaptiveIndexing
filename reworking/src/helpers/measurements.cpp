#ifndef MEASUREMENTS
#define MEASUREMENTS

#include <chrono>
#include <vector>

class Measurements
{
    using time_point = std::__1::chrono::steady_clock::time_point;
public:
    time_point initialization_time;
    std::vector<time_point> query_time;
    std::vector<int64_t> index_height;
    std::vector<int64_t> number_of_nodes;
    std::vector<int64_t> memory_footprint;

    Measurements(){}
    ~Measurements(){}

    time_point time(){
        return std::chrono::steady_clock::now();
    }
};
#endif