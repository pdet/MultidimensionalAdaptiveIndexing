#pragma once

#include "table.hpp"
#include "workload.hpp"
#include <string>

class AbstractGenerator{
    public:

        AbstractGenerator(){}

        virtual void generate(std::string table_path, std::string query_path){
            auto table = generate_table();
            table->save_file(table_path);

            auto workload = generate_workload();
            workload->save_file(query_path);
        }

        virtual unique_ptr<Table> generate_table() = 0;
        virtual unique_ptr<Workload> generate_workload() = 0;
};
