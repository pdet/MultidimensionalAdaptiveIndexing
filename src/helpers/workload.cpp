#include "workload.hpp"
#include "split_string.hpp"
#include <fstream>

Workload::Workload(){}

void Workload::append(Query query){
    queries.push_back(query);
}

Workload Workload::read_file(string path){
    ifstream file(path.c_str(), ios::in);
    if(!file.is_open()){
        cout << "Error *opening* file\n";
        exit(-1);
    }

    auto workload = Workload();

    string low_line;
    string high_line;
    string col_line;
    while(getline(file, low_line)){
        getline(file, high_line);
        getline(file, col_line);

        auto lows = SplitString<float>::split(low_line, " ");
        auto highs = SplitString<float>::split(high_line, " ");
        auto cols = SplitString<size_t>::split(col_line, " ");

        workload.append(
            Query(lows, highs, cols)
        );

    }

    file.close();

    return workload;

}

void Workload::save_file(string path){
    ofstream file(path.c_str(), ios::out);
    for(auto& query : queries){
        auto dimensions = query.predicate_count();
        // Write the lows
        for(size_t j = 0; j < query.predicate_count(); ++j){
           file << query.predicates.at(j).low; 
           if(j + 1 != dimensions)
               file << " ";
        }
        file << std::endl;

        // Write the highs
        for(size_t j = 0; j < query.predicate_count(); ++j){
           file << query.predicates.at(j).high; 
           if(j + 1 != dimensions)
               file << " ";
        }
        file << std::endl;

        // Write the cols
        for(size_t j = 0; j < query.predicate_count(); ++j){
           file << query.predicates.at(j).column; 
           if(j + 1 != dimensions)
               file << " ";
        }
        file << std::endl;
    }
    file.close();
}

size_t Workload::query_count() const{
    return queries.size();
}
