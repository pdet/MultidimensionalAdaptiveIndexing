#ifndef WORKLOAD_H
#define WORKLOAD_H

#include "query.hpp"
#include <cstdint>
#include <vector>
#include <string>

using namespace std;

class Workload 
{
public:
    vector<Query> queries;
    Workload();

    void append(Query query);

    static Workload read_file(string path);

    void save_file(string path); 

    size_t query_count() const;
};
#endif // WORKLOAD_H
