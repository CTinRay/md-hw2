#include "factor-functions.hpp"
#include "type.hpp"
#include <vector>


class FactorGraph {    
private:
    unsigned int nVars;
    struct Factor {
        const std::vector<Index>scope;
        const FactorFunction&function;
    };
    std::vector<std::vector<Index>>varFactors;
    std::vector<Factor>factors;
    Real evalFactor(Index factorIndex,
                    const std::vector<Label>&assignment);
public:
    FactorGraph(unsigned int nVars);
    void addFactor(const std::vector<Index>&scope,
                   const FactorFunction&factorFunction);
    Real evalAt(Index varIndex,
                const std::vector<Label>&assignment);
};

