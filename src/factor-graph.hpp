#include "factor-functions.hpp"
#include "type.hpp"
#include <vector>

#ifndef FACTOR_GRAPH_HPP
#define FACTOR_GRAPH_HPP

class FactorGraph {
    friend class EvalGraph;
private:    
    struct Factor {
        const std::vector<Index>scope;
        const FactorFunction&function;
    };
    std::vector<std::vector<Index> >varFactors;
public:
    std::vector<Factor>factors;
    unsigned int nVars;
    FactorGraph(unsigned int nVars);
    void addFactor(const std::vector<Index>&scope,
                   const FactorFunction&factorFunction);
};


class EvalGraph {
private:
public:
    std::vector< std::vector<std::vector<Label>::iterator> >scopes;
    const FactorGraph&factorGraph;
    std::vector<Label>assignment;
    EvalGraph(const FactorGraph&factorGraph);
    EvalGraph(const EvalGraph&graph);    
    Real evalAt(Index varIndex) const;        
};


#endif


