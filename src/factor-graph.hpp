#include "factor-functions.hpp"
#include "type.hpp"
#include <vector>


class FactorGraph {    
private:
    struct Factor {
        const std::vector<Index>scope;
        const FactorFunction&function;
    };
    std::vector<std::vector<Index>>varFactors;
    std::vector<Factor>factors;
    Real evalFactor(Index factorIndex,
                    const std::vector<Label>&assignment);
public:
    unsigned int nVars;
    FactorGraph(unsigned int nVars);
    void addFactor(const std::vector<Index>&scope,
                   const FactorFunction&factorFunction);
    Real evalAt(Index varIndex,
                const std::vector<Label>&assignment) const;
};

