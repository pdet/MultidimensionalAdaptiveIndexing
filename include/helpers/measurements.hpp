#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <chrono>
#include <vector>
#include <numeric>
#include <sqlite3.h>

class Measurements
{
    using time_point = std::__1::chrono::steady_clock::time_point;
public:
    double initialization_time;
    std::vector<double> adaptation_time;
    std::vector<double> query_time;
    std::vector<size_t> max_height;
    std::vector<size_t> min_height;
    std::vector<size_t> number_of_nodes;
    std::vector<size_t> memory_footprint;

    Measurements();
    ~Measurements();

    time_point time();

    static double difference(time_point end, time_point start);

    double average_adaptation_time(){
        return average(adaptation_time);
    }

    double average_query_time(){
        return average(query_time);
    }

    double average(std::vector<double> v){
        return std::accumulate(v.begin(), v.end(), 0.0)/(double)v.size();
    }

    void save_to_sql(std::string db_name, int repetition, std::string alg_name);

    std::string quotes(std::string s);

private:

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    void create_table();

    void insert(size_t repetition, std::string alg_name);

    static int callback(void *NotUsed, int argc, char **argv, char **azColName);
};
#endif // MEASUREMENTS_H