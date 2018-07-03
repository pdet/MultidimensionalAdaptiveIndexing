#include <string>

#include "util/file_manager.h"
#include "util/structs.h"

using namespace std;

int main(int argc, char **argv){
    string DATA_FILE_PATH = "./benchmarks/TPCH/lineitem.csv";

    Table t;
    tpch_loadData(t, DATA_FILE_PATH);
}