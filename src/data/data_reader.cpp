#include "data_reader.hpp"

using namespace std;

unique_ptr<Table> DataReader::read_table(const string &data_path){
    ifstream file(data_path.c_str(), ios::in);
    if(!file.is_open()){
        cout << "Error *opening* data file\n";
        exit(-1);
    }
    string line;
    if(!getline(file, line)){
        cout << "Error *reading* data file\n";
        exit(-1);
    }

    auto row = split<float>(line, ' ');
    auto table = make_unique<Table>(row.size());
    table->append(row);

    while(getline(file,line)){
        row = split<float>(line, ' ');
        table->append(row);
    }

    file.close();

    return table;
}

vector<Query> DataReader::read_workload(const string &workload_path){
    ifstream file(workload_path.c_str(), ios::in);
    if(!file.is_open()){
        cout << "Error *opening* workload file\n";
        exit(-1);
    }

    auto workload = vector<Query>();

    string low_line;
    string high_line;
    string col_line;
    while(getline(file, low_line)){
        getline(file, high_line);
        getline(file, col_line);

        auto lows = split<float>(low_line, ' ');
        auto highs = split<float>(high_line, ' ');
        auto cols = split<int64_t>(col_line, ' ');

        workload.push_back(
                Query(lows, highs, cols)
                );

    }

    file.close();

    return workload;
}


