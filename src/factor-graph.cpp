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

EvalGraph::EvalGraph(const FactorGraph&factorGraph)
    :factorGraph(factorGraph),
     scopes(std::vector< std::vector<std::vector<Label>::iterator> >
            (factorGraph.factors.size())),
     assignment(std::vector<Label>(factorGraph.nVars))
{    
    
    for (auto i = 0u; i < factorGraph.factors.size(); ++i) {
        for (auto varInd: factorGraph.factors[i].scope) {
            scopes[i].push_back(assignment.begin() + varInd);
        }
    }
}

EvalGraph::EvalGraph(const EvalGraph&evalGraph)
    :factorGraph(evalGraph.factorGraph),
     scopes(std::vector< std::vector<std::vector<Label>::iterator> >
            (factorGraph.factors.size())),
     assignment(std::vector<Label>(factorGraph.nVars))
{    
    
    for (auto i = 0u; i < factorGraph.factors.size(); ++i) {
        for (auto varInd: factorGraph.factors[i].scope) {
            scopes[i].push_back(assignment.begin() + varInd);
        }
    }
}

Real EvalGraph::evalAt(Index varIndex) const{
    Real potential = RealMulId;
    for (auto varFactor: factorGraph.varFactors[varIndex]) {
        potential *= factorGraph.factors[varFactor]
            .function.eval(scopes[varFactor]);
    }
    return potential;
}
