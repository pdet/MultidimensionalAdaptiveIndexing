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

    vector<vector<size_t>> result_sizes(algorithms.size());

    cout << "Testing algorithms: " << endl;

    for(size_t i = 0; i < algorithms.size(); ++i){
        auto algorithm = move(algorithms.at(i));
        cout << "Testing " << algorithm->name() << endl;
        algorithm->initialize(table);
        for(size_t j = 0; j < workload.size(); ++j){
            algorithm->adapt_index(workload.at(j));
            auto result = algorithm->range_query(workload.at(j));
            result_sizes.at(i).push_back(result->row_count());
        }
    }

    for(size_t j = 0; j < algorithms.size(); ++j){
        for(size_t i = 0; i < workload.size(); ++i){
            cout << result_sizes.at(i).at(j) << " ";
        }
        cout << endl;
    }

    return 0;
}