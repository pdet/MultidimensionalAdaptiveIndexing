#include "data_reader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

shared_ptr<Table> DataReader::read_table(const string &data_path){
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

    auto row = split(line, ' ');
    auto table = make_shared<Table>(row.size());
    table->append(row);

    while(getline(file,line)){
        row = split(line, ' ');
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

        auto lows = split(low_line, ' ');
        auto highs = split(high_line, ' ');
        auto cols = split(col_line, ' ');

        workload.push_back(
                Query(lows, highs, cols)
                );

    }

    file.close();

    return workload;
}

vector<float> DataReader::split(const string& s, char delimiter)
{
    string buf;                         // Have a buffer string
    stringstream ss(s.c_str());         // Insert the string into a stream

    vector<float> tokens;               // Create vector to hold our words

    while (ss >> buf)
        tokens.push_back(atof(buf.c_str()));

    return tokens;
}
