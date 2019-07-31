#include "index_factory.hpp"
#include "../data/data_reader.cpp"
#include <string>
#include <vector>
#include <iostream>
#include <numeric>

using namespace std;

int main(){
    const string workload_path = "../src/test/queries.sql";
    const string table_path = "../src/test/data.sql";

    auto table = DataReader::read_table(table_path);
    auto workload = DataReader::read_workload(workload_path);

    auto algorithms = IndexFactory::allIndexes();

    auto baseline = IndexFactory::baseline_index();

    vector<vector<size_t> > result_sizes(algorithms.size());

    cout << "Testing algorithms: " << endl;

    for(size_t i = 0; i < algorithms.size(); ++i){
        auto algorithm = algorithms.at(i);
        cout << "   '-" << algorithm->name() << endl;
        algorithm->initialize(table);
        for(size_t j = 0; j < workload.size(); ++j){
            algorithm->adapt_index(workload.at(j));
            auto result = algorithm->range_query(workload.at(j));
            result_sizes.at(i).push_back(result->row_count());
        }
    }

    cout << "   '-Baseline (" << baseline->name() << ")" << endl;

    vector<size_t> baseline_results(workload.size());

    baseline->initialize(table);
    for(size_t j = 0; j < workload.size(); ++j){
        baseline->adapt_index(workload.at(j));
        auto result = baseline->range_query(workload.at(j));
        baseline_results.at(j) = result->row_count();
    }

    vector<size_t> number_of_different(algorithms.size(), 0);

    for(size_t query_index = 0; query_index < workload.size(); ++query_index){
        for(size_t alg_index = 0; alg_index < algorithms.size(); ++alg_index){
            if(result_sizes.at(alg_index).at(query_index) != baseline_results.at(query_index))
                number_of_different.at(alg_index)++;
        }
    }

    cout << "Number of wrongs:" << endl;

    for(size_t alg_index = 0; alg_index < algorithms.size(); ++alg_index){
            cout << algorithms.at(alg_index)->name() <<": " << number_of_different.at(alg_index) << endl;
    }
    cout << endl;

    double sum = accumulate(baseline_results.begin(), baseline_results.end(), 0.0);
    auto avg = sum / (double) baseline_results.size();
    cout << "Average baseline result size: " << avg << endl;

    cout << endl;

    algorithms.push_back(baseline);

    for(auto alg : algorithms){
        cout << "----" << alg->name() << "----" << endl;
        cout << "Initialization Time (seconds): " << alg->measurements->initialization_time << endl;
        cout << "Average Adaptation Time (seconds): " << alg->measurements->average_adaptation_time() << endl;
        cout << "Average Query Time (seconds): " << alg->measurements->average_query_time() << endl;
    }

    return 0;
}