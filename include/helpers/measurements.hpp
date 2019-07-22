#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <chrono>
#include <vector>

class Measurements
{
    using time_point = std::__1::chrono::steady_clock::time_point;
    using duration = std::__1::chrono::duration<double>;
public:
    duration initialization_time;
    std::vector<duration> adaptation_time;
    std::vector<duration> query_time;
    std::vector<size_t> max_height;
    std::vector<size_t> min_height;
    std::vector<size_t> number_of_nodes;
    std::vector<size_t> memory_footprint;

    Measurements();
    ~Measurements();

    time_point time();

    static std::chrono::nanoseconds difference(time_point end, time_point start);
};
#endif // MEASUREMENTS_H