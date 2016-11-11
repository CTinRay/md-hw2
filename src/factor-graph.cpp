#include "factor-graph.hpp"
#include <cassert>

FactorGraph::FactorGraph(unsigned int nVars)
    :varFactors(std::vector<std::vector<Index>>(nVars)),
     nVars(nVars){}


void FactorGraph::addFactor(const std::vector<Index>&scope,
                            const FactorFunction&factorFunction) {
    FactorGraph::Factor factor {
        scope, factorFunction
    };
    factors.push_back(factor);
    for (auto ind: scope) {
        assert(ind < nVars);
        varFactors[ind].push_back(factors.size() - 1);
    }
}


Real FactorGraph::evalFactor(Index factorIndex,
                             const std::vector<Label>&assignment) const{
    std::vector<Label>args;
    for (auto varIndex: factors[factorIndex].scope) {
        args.push_back(assignment[varIndex]);
    }
    return factors[factorIndex].function.eval(args);
}

Real FactorGraph::evalAt(Index varIndex, 
                         const std::vector<Label>&assignment) const{
    Real potential = RealMulId;
    for (auto varFactor: varFactors[varIndex]) {
        potential *= evalFactor(varFactor, assignment);
    }
    return potential;
}
