#ifndef ALL_HYPERPLANES_NARROW_H
#define ALL_HYPERPLANES_NARROW_H

#include "abstract_cracking_kd_tree.hpp"
#include <map>
#include <string>

class AllHyperplanesNarrow : public AbstractCrackingKDTree 
{
public:
    AllHyperplanesNarrow(std::map<std::string, std::string> config);
    ~AllHyperplanesNarrow();

    string name() override{
        return "AllHyperplanesNarrow";
    }

    void adapt_index(Query& query) override;
};
#endif // ALL_HYPERPLANES_NARROW_H
