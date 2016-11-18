#include "target-functions.hpp"
#include <iostream>

MarginalProb::MarginalProb(Index varInd)
    :varInd(varInd){}


Real MarginalProb::eval(const EvalGraph&graph) const {
    return graph.assignment[varInd] == Label::positive ? 1 : 0;
}    


ExpectFactorF::ExpectFactorF(Index featureInd,Real phSum, Real plSum,
                             const std::vector<Index>&varHInds,
                             const std::vector<Index>&varLInds)
    :featureInd(featureInd),
     varHInds(varHInds),
     varLInds(varLInds),
     phSum(phSum),
     plSum(plSum){}
    
Real ExpectFactorF::eval(const EvalGraph&graph) const{
    unsigned int countH = 0, countL = 0;
    for (auto ind: varHInds) {
        if (graph.assignment[ind] == Label::positive) {
            countH += 1;
        }            
    }
    for (auto ind: varLInds) {
        if (graph.assignment[ind] == Label::positive) {
            countL += 1;
        }            
    }        
    if (countH == 0 && countL == 0) {
        return 0;
    }
    Real sum = RealAddId;
    for (Index i = 0; i < graph.factorGraph.factors.size(); i++) {
        if (graph.factorGraph.factors[i].function.factorType == FactorType::f) {
            const FFactorFunction*f =
                (const FFactorFunction*) &graph.factorGraph.factors[i].function;
            sum += f -> evalF(featureInd, graph.scopes[i]);
            // if (product <= 0) {
            //     std::cerr << "product < 0: " << product << std::endl;
            // }
        }        
    }
    return (countH * sum) / phSum - (countL * sum) / plSum ;
}    


ExpectFactorG::ExpectFactorG(GFactorFunction*function,Real phSum, Real plSum,
                             const std::vector<Index>&varHInds,
                             const std::vector<Index>&varLInds)
    :function(function),
     varHInds(varHInds),
     varLInds(varLInds),
     phSum(phSum),
     plSum(plSum){}
    
Real ExpectFactorG::eval(const EvalGraph&graph) const{
    unsigned int countH = 0, countL = 0;
    for (auto ind: varHInds) {
        if (graph.assignment[ind] == Label::positive) {
            countH += 1;
        }            
    }
    for (auto ind: varLInds) {
        if (graph.assignment[ind] == Label::positive) {
            countL += 1;
        }            
    }        
    if (countH == 0 && countL == 0) {
        return 0;
    }
    Real sum = RealAddId;
    for (Index i = 0; i < graph.factorGraph.factors.size(); i++) {
        if (&graph.factorGraph.factors[i].function == function) {
            const GFactorFunction*g =
                (const GFactorFunction*) &graph.factorGraph.factors[i].function;
            sum += g -> evalF(graph.scopes[i]);
        }
    }
    return (countH * sum) / phSum - (countL * sum) / plSum ;
}    


ExpectFactorH::ExpectFactorH(Real phSum, Real plSum,
                             const std::vector<Index>&varHInds,
                             const std::vector<Index>&varLInds)
    :varHInds(varHInds),
     varLInds(varLInds),
     phSum(phSum),
     plSum(plSum){}
    
Real ExpectFactorH::eval(const EvalGraph&graph) const{
    unsigned int countH = 0, countL = 0;
    for (auto ind: varHInds) {
        if (graph.assignment[ind] == Label::positive) {
            countH += 1;
        }            
    }
    for (auto ind: varLInds) {
        if (graph.assignment[ind] == Label::positive) {
            countL += 1;
        }            
    }        
    if (countH == 0 && countL == 0) {
        return 0;
    }
    Real sum = RealAddId;
    for (Index i = 0; i < graph.factorGraph.factors.size(); i++) {
        if (graph.factorGraph.factors[i].function.factorType == FactorType::h) {
            const HFactorFunction*g =
                (const HFactorFunction*) &graph.factorGraph.factors[i].function;
            sum += g -> evalF(graph.scopes[i]);
        }
    }
    return (countH * sum) / phSum - (countL * sum) / plSum ;
}    
