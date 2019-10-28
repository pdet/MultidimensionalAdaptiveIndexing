#ifndef STANDARD_CRACKING_H
#define STANDARD_CRACKING_H

#include "abstract_index.hpp"
#include <string>
#include <vector>
#include <set>
#include <string>
#include <map>


class StandardCrackingNode{
    public:
        float value;
        int64_t position;
        StandardCrackingNode(){
            value = 0;
            position = 0;
        }

        StandardCrackingNode(float v, int64_t p){
            value = v;
            position = p;
        }

        StandardCrackingNode(const StandardCrackingNode &other){
            value = other.value;
            position = other.position;
        }

        bool operator<(const StandardCrackingNode &other) const{
            return value < other.value;
        }
        
        bool operator=(const StandardCrackingNode &other) const{
            return value == other.value;
        }
};

class StandardCracking : public  AbstractIndex
{
    public:
        StandardCracking(std::map<std::string, std::string> config);
        ~StandardCracking(); 

        std::string name() override{
            return "Standard Cracking";
        }
        void initialize(Table* table_to_copy) override;

        void adapt_index(Query& query) override;

        Table range_query(Query& query) override;
    private:
        std::vector<Table> cracker_columns;
        std::vector<std::set<StandardCrackingNode>> index;
        int64_t minimum_partition_size = 100;

        void adapt(Query &query);

        std::vector<char> search(Query &query);


        std::pair<int64_t, int64_t> search_partition(std::set<StandardCrackingNode> s, float value);

        int64_t get_node_count();
        int64_t biggest_index();
        int64_t smallest_index();
};
#endif // STANDARD_CRACKING_H
