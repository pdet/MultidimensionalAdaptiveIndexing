#include "measurements.hpp"
#include <fstream>
#include <iostream>

Measurements::Measurements(){}
Measurements::~Measurements(){}

double Measurements::time(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec + time.tv_usec/1e6;
}

double Measurements::difference(double end, double start){
    return end - start;
}

void Measurements::save(std::string csv_name, int repetition, std::string alg_name){
    bool file_exists = exists(csv_name);
    std::fstream csv_file(csv_name, std::ios::out | std::ios::app);

    if( !csv_file.is_open() ) {
        std::cout << "Can't open file: " << csv_name << std::endl;
        return;
    }

    if(!file_exists){
        // Create header of CSV
        csv_file << "NAME,INITIALIZATION_TIME,ADAPTATION_TIME,QUERY_TIME,MIN_HEIGHT,MAX_HEIGHT,NUMBER_OF_NODES,MEMORY_FOOTPRINT,TUPLES_SCANNED,REPETITION\n";
    }

    // Append the results
    double init_time = initialization_time;
    for(int64_t i = 0; i < adaptation_time.size(); ++i){
        auto line =  '"' + alg_name + "\", " \
                   + std::to_string(init_time) + ", " \
                   + std::to_string(adaptation_time.at(i)) + ", " \
                   + std::to_string(query_time.at(i)) + ", " \
                   + std::to_string(min_height.at(i)) + ", " \
                   + std::to_string(max_height.at(i)) + ", " \
                   + std::to_string(number_of_nodes.at(i)) + ", " \
                   + std::to_string(memory_footprint.at(i)) + ", " \
                   + std::to_string(tuples_scanned.at(i)) + ", " \
                   + std::to_string(repetition);

        init_time = 0;
        csv_file << line << std::endl;
    }

    csv_file.close();
}

inline bool Measurements::exists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}
