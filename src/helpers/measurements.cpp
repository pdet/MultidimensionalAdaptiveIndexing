#include "measurements.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

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

void Measurements::append(std::string key, std::string value){
    measurements[key].push_back(value);
}

std::vector<std::string> Measurements::get_headers(
    std::map<std::string, std::vector<std::string>> m
){
    std::vector<std::string> headers;
    for(auto const &element : m){
        headers.push_back(element.first);
    }
    std::sort(headers.begin(), headers.end());
    return headers;
}

void Measurements::save(std::string csv_name, int repetition){
    bool file_exists = exists(csv_name);
    std::fstream csv_file(csv_name, std::ios::out | std::ios::app);

    if( !csv_file.is_open() ) {
        std::cout << "Can't open file: " << csv_name << std::endl;
        return;
    }

    auto headers = get_headers(measurements);

    if(!file_exists){
        // Create header of CSV
        for(auto const &header : headers)
            csv_file << header << ",";
        csv_file << "repetition";
        csv_file << std::endl;
    }

    // Append the results
    for(size_t i = 0; i < measurements[headers[0]].size(); ++i){
        for(auto const& header : headers){
            if(measurements[header].size() <= i)
                csv_file << "0,";
            else
                csv_file << measurements[header][i] << ",";
        }
        csv_file << std::to_string(repetition);
        csv_file << std::endl;
    }

    csv_file.close();
}

inline bool Measurements::exists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}
