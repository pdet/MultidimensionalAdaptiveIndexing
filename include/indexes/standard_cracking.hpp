#ifndef STANDARD_CRACKING_H
#define STANDARD_CRACKING_H

#include "abstract_index.hpp"
#include <string>
#include <vector>
#include <set>

class StandardCrackingNode{
    public:
        float value;
        size_t position;
        StandardCrackingNode(){
            value = 0;
            position = 0;
        }

        StandardCrackingNode(float v, size_t p){
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
        StandardCracking();
        ~StandardCracking(); 

        std::string name() override{
            return "Standard Cracking";
        }
        void initialize(const shared_ptr<Table> table_to_copy) override;

        void adapt_index(Query& query) override;

        shared_ptr<Table> range_query(Query& query) override;
    private:
        std::vector<Table> cracker_columns;
        std::vector<std::set<StandardCrackingNode>> index;
        const size_t minimum_partition_size = 100;

        void adapt(Query &query);

        std::vector<char> search(Query &query);

        size_t get_node_count();
        size_t biggest_index();
        size_t smallest_index();
};
#endif // STANDARD_CRACKING_H
