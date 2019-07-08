#ifndef DATA_READER
#define DATA_READER

#include "../helpers/helpers.cpp"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class DataReader
{
    public:
        static shared_ptr<Table> read_table(const string data_path){
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

        static vector<shared_ptr<Query>> read_workload(const string workload_path){
            ifstream file(workload_path.c_str(), ios::in);
            if(!file.is_open()){
                cout << "Error *opening* workload file\n";
                exit(-1);
            }

            file.close();

            auto workload = vector<shared_ptr<Query>>();

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
                    make_shared<Query>(lows, highs, cols)
                );

            }

            return workload;
        }

    private:
        static vector<float> split(const string& s, char delimiter)
        {
            string buf;                         // Have a buffer string
            stringstream ss(s.c_str());         // Insert the string into a stream

            vector<float> tokens;               // Create vector to hold our words

            while (ss >> buf)
                tokens.push_back(atof(buf.c_str()));

            return tokens;
        }
};
#endif