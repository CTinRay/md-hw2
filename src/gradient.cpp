#include "factor-graph.hpp"
#include "target-functions.hpp"
#include "gibbs-sampler.hpp"
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>


std::random_device rd;
std::mt19937_64 gen(rd());    


class MarginalProb: public TargetFunction{
public:    
    Index varInd;
    MarginalProb(Index varInd)
        :varInd(varInd){}
    Real eval(const EvalGraph&graph) const{
        return graph.assignment[varInd] == Label::positive ? 1 : 0;
    }    
};


class ExpectFactorF: public TargetFunction{
private:
    Index featureInd;
    const std::vector<Index>&varHInds;
    const std::vector<Index>&varLInds;
    const Real phSum, plSum;
public:    
    ExpectFactorF(Index featureInd,Real phSum, Real plSum,
                  const std::vector<Index>&varHInds,
                  const std::vector<Index>&varLInds)
        :featureInd(featureInd),
         varHInds(varHInds),
         varLInds(varLInds),
         phSum(phSum),
         plSum(plSum){}
    
    Real eval(const EvalGraph&graph) const{
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
        Real product = RealMulId;
        for (Index i = 0; i < graph.factorGraph.factors.size(); i++) {
            if (graph.factorGraph.factors[i].function.factorType == FactorType::f) {
                const FFactorFunction*f =
                    (const FFactorFunction*) &graph.factorGraph.factors[i].function;
                product *= f -> evalF(featureInd, graph.scopes[i]);
            }
        }
        return (countH * product) / phSum - (countL * product) / plSum ;
    }    
};


class ExpectFactorG: public TargetFunction{
private:
    GFactorFunction*function;
    const std::vector<Index>&varHInds;
    const std::vector<Index>&varLInds;
    const Real phSum, plSum;
public:    
    ExpectFactorG(GFactorFunction*function,Real phSum, Real plSum,
                  const std::vector<Index>&varHInds,
                  const std::vector<Index>&varLInds)
        :function(function),
         varHInds(varHInds),
         varLInds(varLInds),
         phSum(phSum),
         plSum(plSum){}
    
    Real eval(const EvalGraph&graph) const{
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
        Real product = RealMulId;
        for (Index i = 0; i < graph.factorGraph.factors.size(); i++) {
            if (&graph.factorGraph.factors[i].function == function) {
                const FFactorFunction*g =
                    (const FFactorFunction*) &graph.factorGraph.factors[i].function;
                product *= g -> evalF(graph.scopes[i]);
            }
        }
        return (countH * product) / phSum - (countL * product) / plSum ;
    }    
};


class ExpectFactorH: public TargetFunction{
private:
    const std::vector<Index>&varHInds;
    const std::vector<Index>&varLInds;
    const Real phSum, plSum;
public:    
    ExpectFactorH(Real phSum, Real plSum,
                  const std::vector<Index>&varHInds,
                  const std::vector<Index>&varLInds)
        :varHInds(varHInds),
         varLInds(varLInds),
         phSum(phSum),
         plSum(plSum){}
    
    Real eval(const EvalGraph&graph) const{
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
        Real product = RealMulId;
        for (Index i = 0; i < graph.factorGraph.factors.size(); i++) {
            if (graph.factorGraph.factors[i].function.factorType == FactorType::h) {
                const FFactorFunction*g =
                    (const FFactorFunction*) &graph.factorGraph.factors[i].function;
                product *= g -> evalF(graph.scopes[i]);
            }
        }
        return (countH * product) / phSum - (countL * product) / plSum ;
    }    
};


void gradient(unsigned int batchSize, Real rate, Real converge, const FactorGraph&factorGraph) {
    std::vector<TargetFunction*>marginalProbs;
    for (Index i = 0; i < factorGraph.nVars; ++i) {
        marginalProbs.push_back(new MarginalProb(i));
    }    

    std::vector<GFactorFunction*>gFactorFunctions;
    
    Real gradientNorm;
    do {
        // random sample batch of candidates
        std::shuffle (marginalProbs.begin(), marginalProbs.end(), gen);   
        std::vector<TargetFunction*>batch;
        std::copy(marginalProbs.begin(), marginalProbs.begin() + batchSize,
                  std::back_inserter(batch));

        // Sample marginal probability of candidates
        GibbsSampler marginSampler(batch, factorGraph);
        auto probs = marginSampler.doSample(16, 100, 1.001);

        // Start to spliting higher/lower
        auto order = std::vector<Index>(batchSize);
        {// Generate {0, 1, 2, ..., n - 1}
            Index n = 0;
            std::generate_n(std::back_inserter(order),
                        batchSize, [&n](){ return n++; });
        }        
        std::sort(order.begin(), order.end(),
                  [&probs](const Index a, const Index b){
                      return probs[a] > probs[b];
                  });

        std::vector<Index>hVarInds;
        for (Index i = 0; i < batchSize / 2; ++i) {
            hVarInds.push_back(((MarginalProb*) batch[order[i]]) -> varInd);
        }
        
        std::vector<Index>lVarInds;
        for (Index i = batchSize / 2; i < batchSize; ++i) {
            lVarInds.push_back(((MarginalProb*) batch[order[i]]) -> varInd);
        }

        Real phSum = RealAddId;
        for (Index i = 0; i < batchSize / 2; ++i) {
            phSum += probs[order[i]];
        }

        Real plSum = RealAddId;
        for (Index i = batchSize / 2 + 1; i < batchSize; ++i) {
            plSum += probs[order[i]];
        }
        
        std::vector<TargetFunction*>factors;
        for (Index i = 0; i < FFactorFunction::alpha.size(); ++i) {
            factors.push_back(new ExpectFactorF(i, phSum, plSum, hVarInds, lVarInds));
        }
        for (Index i = 0; i < gFactorFunctions.size(); ++i) {
            factors.push_back(new ExpectFactorG(gFactorFunctions[i],
                                                phSum, plSum, hVarInds, lVarInds));
        }
        factors.push_back(new ExpectFactorH(phSum, plSum, hVarInds, lVarInds));
        
        // Sample marginal probability of candidates
        GibbsSampler gradientSampler(batch, factorGraph);
        auto gradient = gradientSampler.doSample(16, 100, 1.001);

        
        
    } while (gradientNorm > converge);
    
}
