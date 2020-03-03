#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <sys/time.h>
#include <vector>
#include <numeric>
#include <string>
#include <map>

class Measurements
{
public:
    std::map<std::string, std::vector<std::string> > measurements;
    
    Measurements();
    ~Measurements();

    double time();

    static double difference(double  end, double  start);

    std::vector<std::string> get_headers(
            std::map<std::string, std::vector<std::string>> m
            );

    void append(std::string key, std::string value);

    void save(std::string csv_name, int repetition);

private:
    inline bool exists (const std::string& name);

};
#endif // MEASUREMENTS_H
