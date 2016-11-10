#include <vector>
#include "type.hpp"
#include "factor-functions.hpp"


class FactorGraph{
public:
    unsigned int nVars;
    FactorGraph(unsigned int nVars);
    std::vector<Index>varFactors;
    void addFactor(const std::vector<unsigned int>&scope,
                   const FactorFunction&factorFunction);
                      

};

