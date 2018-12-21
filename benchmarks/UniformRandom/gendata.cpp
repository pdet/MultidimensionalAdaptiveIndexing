//
// Created by PHolanda on 17/12/17.
//


#include <vector>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <array>

using namespace std;

typedef array<int64_t, 3> predicate;
typedef vector<predicate> query;

void randomData(vector<int64_t> &data,int64_t COLUMN_SIZE, int64_t UPPERBOUND){
    for(int i = 0; i < COLUMN_SIZE; i++) {
        data.push_back(rand() % UPPERBOUND);
    }
}

// Stores the data in a column format in a binary file
void generateColumn(
    int64_t COLUMN_SIZE, int64_t UPPERBOUND,
    string COLUMN_FILE_PATH, int64_t NUMBER_OF_COLUMNS
){
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;


    for(size_t i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        vector<int64_t> data;
        start = chrono::system_clock::now();
        randomData(data, COLUMN_SIZE,  UPPERBOUND);
        end = chrono::system_clock::now();

        FILE* f = fopen(COLUMN_FILE_PATH.c_str(), "a+");
        fwrite(&data[0], sizeof(int64_t), COLUMN_SIZE, f);
        fclose(f);

        cout << "Creating column " << i << \
        " data: " << (end-start).count() << "s\n";
    }
}

void randomDataQuery(
    query &query,
    int64_t UPPERBOUND,
    double SELECTIVITY_PERCENTAGE,
    vector<int64_t> columns
){
    int64_t elements_using_selectivity = UPPERBOUND*SELECTIVITY_PERCENTAGE;
    int64_t maxLeftQueryVal = UPPERBOUND - elements_using_selectivity ;
    for (int i = 0; i < columns.size(); i++) {
        int64_t low = rand() % maxLeftQueryVal;
        int64_t high = low + elements_using_selectivity;
        int64_t column = columns.at(i);

        array<int64_t, 3> predicate = {low, high, column};
        query.push_back(predicate);
    }
}

vector<int64_t> columnsInQuery(
    int64_t NUMBER_OF_COLUMNS, int64_t MAX_COLUMNS_PER_QUERY
){
    vector<int64_t> columns (NUMBER_OF_COLUMNS);
    iota(columns.begin(), columns.end(), 0);
    random_shuffle(columns.begin(), columns.end());
    columns.resize(MAX_COLUMNS_PER_QUERY);
    return columns;
}

void transformQueryIntoOneSided(query &query){

    for(size_t i = 0; i < query.size(); i++)
    {
        if((rand() % 2) == 1){
            query.at(i).at(0) = -1;
        } else {
            query.at(i).at(1) = -1;
        }
    }

}

void generateQuery(
    int64_t NUM_QUERIES, int64_t UPPERBOUND,
    double SELECTIVITY_PERCENTAGE, double ONE_SIDED_PERCENTAGE,
    int64_t COLUMN_SIZE, string QUERIES_FILE_PATH,
    int64_t NUMBER_OF_COLUMNS, int64_t MAX_COLUMNS_PER_QUERY
){
    cout << "Creating queries\n";

    FILE* f = fopen(QUERIES_FILE_PATH.c_str(), "a+");
    // <low, high, column>
    vector<query> queries(NUM_QUERIES);

    // Generates the random queries
    for(size_t i = 0; i < NUM_QUERIES; i++)
    {
        vector<int64_t> columns = columnsInQuery(
            NUMBER_OF_COLUMNS, MAX_COLUMNS_PER_QUERY
        );
        randomDataQuery(
            queries.at(i), UPPERBOUND,
            SELECTIVITY_PERCENTAGE,
            columns
        );
    }

    //Transform parts of it into one-sided
    vector<int64_t> indices (NUM_QUERIES);
    iota(indices.begin(), indices.end(), 0);
    random_shuffle(indices.begin(), indices.end());

    indices.resize((int64_t) NUM_QUERIES * ONE_SIDED_PERCENTAGE);

    for(size_t i = 0; i < indices.size(); i++)
    {
        transformQueryIntoOneSided(queries.at(indices.at(i)));
    }


    for(size_t i = 0; i < queries.size(); i++)
    {
        query q = queries.at(i);
        size_t n_of_predicates = q.size();
        // First write the number of predicates
        fwrite(&n_of_predicates, sizeof(size_t), 1, f);

        // Then write the predicates
        for(size_t j = 0; j < n_of_predicates; j++)
        {
            predicate p = q.at(j);
            fwrite(&p[0], sizeof(int64_t), 1, f);
            fwrite(&p[1], sizeof(int64_t), 1, f);
            fwrite(&p[2], sizeof(int64_t), 1, f);
        }

    }

    fclose(f);
}


void truncate(string file_path) {
    FILE* f = fopen(file_path.c_str(), "w+");
    fclose(f);
}

void print_help(int argc, char** argv) {
    fprintf(stderr, "Unrecognized command line option.\n");
    fprintf(stderr, "Usage: %s [args]\n", argv[0]);
    fprintf(stderr, "   --column-path\n");
    fprintf(stderr, "   --query-path\n");
    fprintf(stderr, "   --selectiviy\n");
    fprintf(stderr, "   --one-sided-ranges\n");
    fprintf(stderr, "   --num-queries\n");
    fprintf(stderr, "   --column-size\n");
    fprintf(stderr, "   --upperbound\n");
    fprintf(stderr, "   --column-number\n");
}

pair<string,string> split_once(string delimited, char delimiter) {
    auto pos = delimited.find_first_of(delimiter);
    return make_pair(delimited.substr(0, pos), delimited.substr(pos+1));
}
int main(int argc, char** argv) {
    //Default Values
    string COLUMN_FILE_PATH =  "column";
    string QUERIES_FILE_PATH = "query";
    double SELECTIVITY_PERCENTAGE = 0.5;
    double ONE_SIDED_PERCENTAGE = 0;
    int64_t NUM_QUERIES = 1000;
    int64_t COLUMN_SIZE = 10000000;
    int64_t UPPERBOUND = COLUMN_SIZE;
    int64_t NUMBER_OF_COLUMNS = 16;
    int64_t MAX_COLUMNS_PER_QUERY = NUMBER_OF_COLUMNS;
    uint SEED = 1098;

    for(int i = 1; i < argc; i++) {
        auto arg = string(argv[i]);
        if (arg.substr(0,2) != "--") {
            print_help(argc, argv);
            exit(EXIT_FAILURE);
        }
        arg = arg.substr(2);
        auto p = split_once(arg, '=');
        auto& arg_name = p.first; auto& arg_value = p.second;
        if (arg_name == "column-path") {
                COLUMN_FILE_PATH = arg_value;
            } else if (arg_name == "query-path") {
                QUERIES_FILE_PATH = arg_value;
            } else if (arg_name == "selectivity") {
                SELECTIVITY_PERCENTAGE = atof(arg_value.c_str());
            } else if (arg_name == "one-side-ranges") {
                ONE_SIDED_PERCENTAGE = atof(arg_value.c_str());
            } else if (arg_name == "num-queries") {
                NUM_QUERIES = atoi(arg_value.c_str());
            } else if (arg_name == "column-size") {
                COLUMN_SIZE = atoi(arg_value.c_str());
            } else if (arg_name == "upperbound") {
                UPPERBOUND = atoi(arg_value.c_str());
            } else if (arg_name == "column-number") {
                NUMBER_OF_COLUMNS = atoi(arg_value.c_str());
            } else if (arg_name == "max-columns-per-query") {
                MAX_COLUMNS_PER_QUERY = atoi(arg_value.c_str());
            } else if (arg_name == "seed"){
                SEED = atoi(arg_name.c_str());
            } else {
                print_help(argc, argv);
                exit(EXIT_FAILURE);
            }
    }

    srand(SEED);

    truncate(COLUMN_FILE_PATH);
    truncate(QUERIES_FILE_PATH);

    generateColumn(COLUMN_SIZE,UPPERBOUND, COLUMN_FILE_PATH, NUMBER_OF_COLUMNS);
    generateQuery(
        NUM_QUERIES, UPPERBOUND, SELECTIVITY_PERCENTAGE,
        ONE_SIDED_PERCENTAGE, COLUMN_SIZE, QUERIES_FILE_PATH,
        NUMBER_OF_COLUMNS, MAX_COLUMNS_PER_QUERY
    );
}
