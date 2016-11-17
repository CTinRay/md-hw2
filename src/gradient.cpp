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
    const std::vector<Index>&varInds;
public:    
    ExpectFactorF(Index featureInd, const std::vector<Index>&varInds)
        :featureInd(featureInd),
         varInds(varInds){}
    Real eval(const EvalGraph&graph) const{
        unsigned int count = 0;
        for (auto ind: varInds) {
            if (graph.assignment[ind] == Label::positive) {
                count += 1;
            }
        }
        if (count == 0) {
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
        return count * product;
    }    
};


void gradient(unsigned int batchSize, Real rate, Real converge, const FactorGraph&factorGraph) {
    std::vector<TargetFunction*>marginalProbs;
    for (Index i = 0; i < factorGraph.nVars; ++i) {
        marginalProbs.push_back(new MarginalProb(i));
    }    

    Real gradientNorm;
    do {
        std::shuffle (marginalProbs.begin(), marginalProbs.end(), gen);   
        std::vector<TargetFunction*>batch;
        std::copy(marginalProbs.begin(), marginalProbs.end(),
                  std::back_inserter(batch));
        GibbsSampler gibbsSampler(batch, factorGraph);
        auto result = gibbsSampler.doSample(16, 100, 1.001);
        Index n = 0;
        auto order = std::vector<Index>(batchSize);
        std::generate_n(std::back_inserter(order),
                        batchSize, [&n](){ return n++; });
        std::sort(order.begin(), order.end(),
                  [&result](const Index a, const Index b){
                      return result[a] > result[b];
                  });            
    } while (gradientNorm > converge);
    
}
