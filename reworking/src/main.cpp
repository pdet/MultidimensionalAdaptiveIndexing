#include "index_factory.cpp"
#include <string>
#include <iostream>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv){
    string workload_path = "workload";
    string data_path = "data";
    string index_algorithm = "Full Scan";
    string sqlite_path = "results_db";

    int c;
    while ((c = getopt (argc, argv, "w:d:i:s:")) != -1)
        switch (c)
        {
        case 'w':
            workload_path = optarg;
            break;
        case 'd':
            data_path = optarg;
            break;
        case 'i':
            index_algorithm = optarg;
            break;
        case 's':
            sqlite_path = optarg;
            break;
        default:
            cout << "Usage: -w <workload_path> -d <data_path> -i <algorithm> -s <sqlite_database_path>";
            return -1;
        }

    auto index = IndexFactory::getIndex(index_algorithm);
    return 0;
}