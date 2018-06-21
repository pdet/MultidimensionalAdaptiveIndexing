//
// Created by PHolanda on 17/12/17.
//


#include <vector>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <cassert>
#include <map>
#include <chrono>
#include <algorithm>
#include "cracking/cracker_index.h"

using namespace std;

int64_t applySelectivity(int64_t leftQuery,double SELECTIVITY_PERCENTAGE, vector<int64_t> *c){
    int64_t selec = 0;
    double aux = c->size()*SELECTIVITY_PERCENTAGE;
    for (size_t i = 0; i < c->size(); i++) {
        if (c->at(i) >= leftQuery)
            selec++;
        if (selec >= aux){
            selec = i;
            break;
        }
    }
    return c->at(selec);
}

void randomData(vector<int64_t> *data,int64_t COLUMN_SIZE, int64_t UPPERBOUND){
    for(int i = 0; i < COLUMN_SIZE; i++) {
        data->push_back(rand() % UPPERBOUND);
    }
}

void randomDataQuery(vector<int64_t> *leftQuery, vector<int64_t> *rightQuery, int64_t UPPERBOUND,
                     int64_t NUM_QUERIES, double SELECTIVITY_PERCENTAGE, double ONE_SIDED_PERCENTAGE,
                     int64_t COLUMN_SIZE, int64_t maxLeftQueryVal) {
    for (int i = 0; i < NUM_QUERIES; i++) {
        if (rand() % 100 < ONE_SIDED_PERCENTAGE) {
            leftQuery->push_back(rand() % UPPERBOUND);
            rightQuery->push_back(-1);
        }
            // Two sided range queries
        else {
            int64_t q1 = rand() % maxLeftQueryVal;
            // int64_t q2 = applySelectivity(q1,SELECTIVITY_PERCENTAGE,orderedColumn);
            int64_t q2 = q1 + COLUMN_SIZE*SELECTIVITY_PERCENTAGE;
            leftQuery->push_back(q1);
            rightQuery->push_back(q2);
        }
    }
}


void sequentialDataQuery(vector<int64_t> *leftQuery,vector<int64_t> *rightQuery, int64_t UPPERBOUND
        ,int64_t NUM_QUERIES,double SELECTIVITY_PERCENTAGE,double ONE_SIDED_PERCENTAGE,
                         vector<int64_t> *orderedColumn, int64_t maxLeftQueryVal){

    int64_t lKey = 0;
    int64_t rKey;
    int64_t jump = UPPERBOUND * 0.01; // Variance of 1%

    for(int i = 0; i < NUM_QUERIES; ++i){
        rKey = applySelectivity(lKey,SELECTIVITY_PERCENTAGE,orderedColumn);
        leftQuery->push_back(lKey);
        rightQuery->push_back(rKey);
        lKey += jump;
        if (lKey > maxLeftQueryVal)
            lKey = rand()%jump;
        // check if query is still in the domain
    }

}


int64_t zipf(double alpha, int64_t n)
{
    static int first = true;      // Static first time flag
    static double c = 0;          // Normalization constant
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value = 0.0;      // Computed exponential value to be returned
    int64_t    i;                     // Loop counter

    // Compute normalization constant on first call only
    if (first == true)
    {
        for (i=1; i<=n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = false;
    }

    // Pull a uniform random number (0 < z < 1)
    do
    {
        z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    while ((z == 0) || (z == 1));

    // Map z to the value
    sum_prob = 0;
    for (i=1; i<=n; i++)
    {
        sum_prob = sum_prob + c / pow((double) i, alpha);
        if (sum_prob >= z)
        {
            zipf_value = i;
            break;
        }
    }

    // Assert that zipf_value is between 1 and N
    assert((zipf_value >=1) && (zipf_value <= n));

    return zipf_value;
}

void generateSkewedData(vector<int64_t> *data,int64_t size, int64_t UPPERBOUND, double ZIPF_ALPHA) {
    // the focus should be in the center of the dataset
    int64_t hotspot = UPPERBOUND  / 2;

    // compute zipf distribution
    typedef map<int64_t, int64_t> result_t;
    typedef result_t::iterator result_iterator_t;

    result_t result;
    for(size_t i = 0; i < size; ++i) {
        int64_t nextValue = zipf(ZIPF_ALPHA, UPPERBOUND);
        result_iterator_t it = result.find(nextValue);
        if(it != result.end()) {
            ++it->second;
        }
        else {
            result.insert(make_pair(nextValue, 1));
        }
    }

    int64_t zoneSize = hotspot / result.size();

    int64_t zone = 0;
    for(result_iterator_t it = result.begin(); it != result.end(); ++it) {
        for(int i = 0; i < it->second; ++i) {
            int64_t direction = rand() % 2 == 0 ? 1 : -1;
            int64_t zoneBegin = hotspot + (zone * zoneSize * direction);
            int64_t zoneEnd = zoneBegin + (zoneSize * direction);
            if(direction == -1) {
                int64_t tmp = zoneBegin;
                zoneBegin = zoneEnd;
                zoneEnd = tmp;
            }
            int64_t predicate = rand() % (zoneEnd - zoneBegin + 1) + zoneBegin;
            data->push_back(predicate);
        }
        ++zone;
    }

    random_shuffle(data->begin(), data->end());

}

void generateSkewedDataForQuery(vector<int64_t> *data,int64_t size, int64_t UPPERBOUND, double ZIPF_ALPHA, int64_t maxLeftQueryVal) {
    // the focus should be in the center of the dataset
    int64_t hotspot = UPPERBOUND  / 2;

    // compute zipf distribution
    typedef map<int64_t, int64_t> result_t;
    typedef result_t::iterator result_iterator_t;

    result_t result;
    for(size_t i = 0; i < size; ++i) {
        int64_t nextValue = zipf(ZIPF_ALPHA, UPPERBOUND);
        result_iterator_t it = result.find(nextValue);
        if(it != result.end()) {
            ++it->second;
        }
        else {
            result.insert(make_pair(nextValue, 1));
        }
    }

    int64_t zoneSize = hotspot / result.size();

    int64_t zone = 0;
    for(result_iterator_t it = result.begin(); it != result.end(); ++it) {
        for(int i = 0; i < it->second; ++i) {
            int64_t direction = rand() % 2 == 0 ? 1 : -1;
            int64_t zoneBegin = hotspot + (zone * zoneSize * direction);
            int64_t zoneEnd = zoneBegin + (zoneSize * direction);
            if(direction == -1) {
                int64_t tmp = zoneBegin;
                zoneBegin = zoneEnd;
                zoneEnd = tmp;
            }
            int64_t predicate = rand() % (zoneEnd - zoneBegin + 1) + zoneBegin;
            while(predicate > maxLeftQueryVal){
                direction = rand() % 2 == 0 ? 1 : -1;
                zoneBegin = hotspot + (zone * zoneSize * direction);
                zoneEnd = zoneBegin + (zoneSize * direction);
                if(direction == -1) {
                    int64_t tmp = zoneBegin;
                    zoneBegin = zoneEnd;
                    zoneEnd = tmp;
                }
                predicate = rand() % (zoneEnd - zoneBegin + 1) + zoneBegin;
            }
            if( UPPERBOUND  / 2 > maxLeftQueryVal){
                cout << "This Selectivity is to high to a centered skewed workload";
                exit(0);
            }
            data->push_back(predicate);
        }
        ++zone;
    }

    random_shuffle(data->begin(), data->end());

}


void skewedDataQuery(vector<int64_t> *leftQuery,vector<int64_t> *rightQuery, int64_t UPPERBOUND
        ,int64_t NUM_QUERIES,double SELECTIVITY_PERCENTAGE,double ONE_SIDED_PERCENTAGE,double ZIPF_ALPHA,
                     vector<int64_t> *orderedColumn, int64_t maxLeftQueryVal) {
    //One sided range queries
    generateSkewedDataForQuery(leftQuery,NUM_QUERIES,UPPERBOUND, ZIPF_ALPHA,maxLeftQueryVal);
    for(int64_t q1 : *leftQuery) {
        if (rand() % 100 < ONE_SIDED_PERCENTAGE) {
            rightQuery->push_back(-1);
        }
        else {
            int64_t q2 = applySelectivity(q1,SELECTIVITY_PERCENTAGE,orderedColumn);
            rightQuery->push_back(q2);
        }
    }
}

void skewedColumn(vector<int64_t> *column,int64_t COLUMN_SIZE, int64_t UPPERBOUND,double ZIPF_ALPHA) {
    generateSkewedData(column,COLUMN_SIZE,UPPERBOUND,ZIPF_ALPHA);
}




vector<int64_t> generateColumn(int64_t COLUMN_SIZE, int64_t UPPERBOUND,
                                    int64_t DATA_COLUMN_PATTERN,double ZIPF_ALPHA, string COLUMN_FILE_PATH) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();
    vector<int64_t> data;
    if(DATA_COLUMN_PATTERN == 1 || DATA_COLUMN_PATTERN == 2)
        randomData(&data, COLUMN_SIZE,  UPPERBOUND);
    else if(DATA_COLUMN_PATTERN == 3)
        skewedColumn(&data, COLUMN_SIZE,  UPPERBOUND,ZIPF_ALPHA);
    end = chrono::system_clock::now();
    elapsed_seconds = end-start;
    FILE* f = fopen(COLUMN_FILE_PATH.c_str(), "a+");
    fwrite(&data[0], sizeof(int64_t), COLUMN_SIZE, f);
    fclose(f);
    cout << "Creating column data: " << elapsed_seconds.count() << "s\n";
    return data;
}

void verifySelectivity(vector<int64_t> *c,vector<int64_t> *l,vector<int64_t> *r, double SELECTIVITY_PERCENTAGE){
    for (size_t i = 0; i < l->size(); i++) {
        int aux = 0;
        for(size_t j = 0; j < c->size(); j++){
            if(c->at(j) > l->at(i) && c->at(j) < r->at(i))
                aux++;
        }
        double selectivity = aux/double(c->size());
        if (!(selectivity < SELECTIVITY_PERCENTAGE + SELECTIVITY_PERCENTAGE * 0.01 && selectivity > SELECTIVITY_PERCENTAGE - SELECTIVITY_PERCENTAGE * 0.01)){
            cout << "Selectivity for query " << i << " is wrong. Got: " << selectivity << " Expected: "<<SELECTIVITY_PERCENTAGE<<" \n";
            //assert(0);
        }
    }



}
void generateQuery(int64_t NUM_QUERIES,
                   int64_t UPPERBOUND,
                   int64_t QUERY_PATTERN,
                   double SELECTIVITY_PERCENTAGE,
                   double ONE_SIDED_PERCENTAGE,
                   double ZIPF_ALPHA,
                   int64_t COLUMN_SIZE,
                   string QUERIES_FILE_PATH) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    FILE* f = fopen(QUERIES_FILE_PATH.c_str(), "a+");
    vector<int64_t> leftQuery;
    vector<int64_t> rightQuery;
    int64_t maxLeftQueryVal = COLUMN_SIZE*SELECTIVITY_PERCENTAGE;
    start = chrono::system_clock::now();

    if(QUERY_PATTERN == 1)
        randomDataQuery(&leftQuery, &rightQuery, UPPERBOUND, NUM_QUERIES, SELECTIVITY_PERCENTAGE, ONE_SIDED_PERCENTAGE,
                        COLUMN_SIZE, maxLeftQueryVal);
    // else if(QUERY_PATTERN == 2)
    //     sequentialDataQuery(&leftQuery, &rightQuery, UPPERBOUND, NUM_QUERIES, SELECTIVITY_PERCENTAGE, ONE_SIDED_PERCENTAGE, orderedColumn, maxLeftQueryVal);
    // else if(QUERY_PATTERN == 3)
    //     skewedDataQuery(&leftQuery, &rightQuery, UPPERBOUND, NUM_QUERIES, SELECTIVITY_PERCENTAGE, ONE_SIDED_PERCENTAGE,ZIPF_ALPHA, orderedColumn, maxLeftQueryVal);
    end = chrono::system_clock::now();
    // verifySelectivity(orderedColumn,&leftQuery,&rightQuery, SELECTIVITY_PERCENTAGE);
    fwrite(&leftQuery[0], sizeof(int64_t), NUM_QUERIES, f);
    fwrite(&rightQuery[0], sizeof(int64_t), NUM_QUERIES, f);
    fclose(f);
    elapsed_seconds = end-start;

    cout << "Creating Query Attr: " << elapsed_seconds.count() << "s\n";
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
    fprintf(stderr, "   --zipf-alpha\n");
    fprintf(stderr, "   --num-queries\n");
    fprintf(stderr, "   --column-size\n");
    fprintf(stderr, "   --upperbound\n");
    fprintf(stderr, "   --queries-pattern\n");
    fprintf(stderr, "   --column-pattern\n");
    fprintf(stderr, "   --column-number\n");
}

std::pair<string,string> split_once(string delimited, char delimiter) {
    auto pos = delimited.find_first_of(delimiter);
    return { delimited.substr(0, pos), delimited.substr(pos+1) };
}
int main(int argc, char** argv) {
    //Default Values
    string COLUMN_FILE_PATH =  "column";
    string QUERIES_FILE_PATH = "query";
    double SELECTIVITY_PERCENTAGE = 0.5;
    double ONE_SIDED_PERCENTAGE = 0;
    double ZIPF_ALPHA = 2.0;
    int64_t NUM_QUERIES = 1000;
    int64_t COLUMN_SIZE = 10000000;
    int64_t UPPERBOUND = COLUMN_SIZE;
    int64_t QUERIES_PATTERN = 1;
    int64_t COLUMN_PATTERN = 1;
    int64_t NUMBER_OF_COLUMNS = 16;

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
            } else if (arg_name == "zipf-alpha") {
                ZIPF_ALPHA = atof(arg_value.c_str());
            } else if (arg_name == "num-queries") {
                NUM_QUERIES = atoi(arg_value.c_str());
            } else if (arg_name == "column-size") {
                COLUMN_SIZE = atoi(arg_value.c_str());
            } else if (arg_name == "upperbound") {
                UPPERBOUND = atoi(arg_value.c_str());
            } else if (arg_name == "queries-pattern") {
                QUERIES_PATTERN = atoi(arg_value.c_str());
            } else if (arg_name == "column-pattern") {
                COLUMN_PATTERN = atoi(arg_value.c_str());
            } else if (arg_name == "column-number") {
                NUMBER_OF_COLUMNS = atoi(arg_value.c_str());
            }  else {
                print_help(argc, argv);
                exit(EXIT_FAILURE);
            }
        }
    

    truncate(COLUMN_FILE_PATH);
    truncate(QUERIES_FILE_PATH);

    vector<int64_t> orderedColumn;
    for (int i = 0; i < NUMBER_OF_COLUMNS; ++i) {
        generateColumn(COLUMN_SIZE,UPPERBOUND,COLUMN_PATTERN,ZIPF_ALPHA, COLUMN_FILE_PATH);
        generateQuery(NUM_QUERIES,UPPERBOUND,QUERIES_PATTERN,SELECTIVITY_PERCENTAGE,ONE_SIDED_PERCENTAGE,ZIPF_ALPHA,COLUMN_SIZE , QUERIES_FILE_PATH);
    }
}
