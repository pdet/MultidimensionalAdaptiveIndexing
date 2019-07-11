#include "../indexes/index_factory.cpp"
#include "../data/data_reader.cpp"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main(){
    const string workload_path = "./test/queries.sql";
    const string table_path = "./test/data.sql";

    auto table = DataReader::read_table(table_path);
    auto workload = DataReader::read_workload(workload_path);

    auto algorithms = IndexFactory::allIndexes();

    algorithms.push_back(IndexFactory::baseline_index());

    vector<vector<size_t>> result_sizes(algorithms.size());

    cout << "Testing algorithms: " << endl;

    for(size_t i = 0; i < algorithms.size(); ++i){
        auto algorithm = move(algorithms.at(i));
        cout << "   '-" << algorithm->name() << endl;
        algorithm->initialize(table);
        for(size_t j = 0; j < workload.size(); ++j){
            algorithm->adapt_index(workload.at(j));
            auto result = algorithm->range_query(workload.at(j));
            result_sizes.at(i).push_back(result->row_count());
        }
    }

    vector<size_t> number_of_different(algorithms.size(), 0);

    for(size_t query_index = 0; query_index < workload.size(); ++query_index){
        for(size_t alg_index = 0; alg_index < algorithms.size(); ++alg_index){
            cout << result_sizes.at(alg_index).at(query_index) << " ";
            if(result_sizes.at(alg_index).at(query_index) != result_sizes.at(0).at(query_index))
                number_of_different.at(alg_index)++;
        }
        cout << endl;
    }

    cout << "-----------------------" << endl;
    for(size_t alg_index = 0; alg_index < algorithms.size(); ++alg_index){
            cout << number_of_different.at(alg_index) << " ";
    }
    cout << endl;
    return 0;
}