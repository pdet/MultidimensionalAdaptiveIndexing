#ifndef MY_GENERATOR_H
#define MY_GENERATOR_H

#include <string>
#include <cstdint>

class MyGenerator{
    public:
        MyGenerator(
                int64_t n_of_rows_,
                int64_t dimensions_,
                float selectivity_,
                int64_t number_of_queries_
                );
        bool generate(const std::string &table_path, const std::string &workload_path);

    private:
        int64_t n_of_rows;
        int64_t dimensions;
        float selectivity;
        int64_t number_of_queries;
};
#endif // MY_GENERATOR_H
