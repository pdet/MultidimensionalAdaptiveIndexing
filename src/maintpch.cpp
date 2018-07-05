#include <string>

#include "util/file_manager.h"
#include "util/structs.h"
#include <array>

using namespace std;

int main(int argc, char **argv){
    string DATA_FILE_PATH = "./benchmarks/TPCH/lineitem.csv";
    string QUERY_FILE_PATH = "./benchmarks/TPCH/queries.csv";

    Table t;
    tpch_loadData(t, DATA_FILE_PATH);

    vector<vector<array<int64_t, 3>>> queries;
    tpch_loadQueries(queries, QUERY_FILE_PATH);
}