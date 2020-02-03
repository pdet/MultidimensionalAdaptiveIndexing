#ifndef ALL_HYPERPLANES_BROAD_H
#define ALL_HYPERPLANES_BROAD_H

#include "abstract_cracking_kd_tree.hpp"
#include <map>
#include <string>

class AllHyperplanesBroad : public AbstractCrackingKDTree 
{
public:
    AllHyperplanesBroad(std::map<std::string, std::string> config);
    ~AllHyperplanesBroad();

    string name() override{
        return "AllHyperplanesBroad";
    }

    void adapt_index(Query& query) override;
};
#endif // ALL_HYPERPLANES_BROAD_H
