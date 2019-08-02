#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <sys/time.h>
#include <vector>
#include <numeric>
#include <sqlite3.h>
#include <string>

class Measurements
{
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

    double time();

    static double difference(double  end, double  start);

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