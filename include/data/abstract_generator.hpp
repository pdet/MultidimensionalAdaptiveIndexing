#ifndef ABSTRACT_GENERATOR_H
#define ABSTRACT_GENERATOR_H

#include "table.hpp"
#include "workload.hpp"
#include <string>

class AbstractGenerator{
    protected:
        unique_ptr<Table> table;
        unique_ptr<Workload> workload;
    public:

        AbstractGenerator(){}

        virtual void generate(std::string table_path, std::string query_path) = 0;
};
#endif // ABSTRACT_GENERATOR_H
