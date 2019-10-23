#ifndef MEDIANS_H
#define MEDIANS_H

#include <string>
#include <cstdint>

class Medians{
    public:
        Medians(
                int64_t n_of_rows_,
                int64_t dimensions_
                );
        void generate(const std::string &table_path, const std::string &workload_path);

    private:
        int64_t n_of_rows;
        int64_t dimensions;
};
#endif // MEDIANS_H
