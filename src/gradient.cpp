#include "factor-graph.hpp"
#include "target-functions.hpp"
#include "gibbs-sampler.hpp"
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>
#include <cassert>

std::random_device rd;
std::mt19937_64 gen(rd());    


void updateWeights(long double rate,
                   const std::vector<GFactorFunction*>&gfs,
                   const std::vector<Real>&gradient){
    unsigned int i = 0;
    while (i < FFactorFunction::alpha.size()) {
        FFactorFunction::alpha[i] += rate * gradient[i];
        i += 1;
    }
    
    for (unsigned int j = 0; j < gfs.size(); ++j) {
        gfs[j] -> beta += rate * gradient[i];
        i += 1;
    }

    HFactorFunction::gamma += rate * gradient[i];
    assert(i == gradient.size() - 1);
}


long double norm(const std::vector<Real>&gradient) {
    Real sum = RealAddId;
    for (auto g: gradient) {
        sum += g * g;
    }
    return std::sqrt((long double)sum);
}


void gradientAscend(unsigned int batchSize, long double rate, Real converge, const FactorGraph&factorGraph) {
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

        updateWeights(rate, gFactorFunctions, gradient);
        gradientNorm = norm(gradient);
            
    } while (gradientNorm > converge);
    
}
