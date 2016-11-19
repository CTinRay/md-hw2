#include "gradient.hpp"
#include <ctime>
#include <random>
#include <algorithm>
#include <cassert>
#include <iostream>

std::random_device rd3;
std::mt19937_64 gen3(rd3());    


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

void printWeights(const std::vector<GFactorFunction*>&gfs) {

    std::cout << "weights = ";
    for (auto a : FFactorFunction::alpha) {
        std::cout << a << " ";
    }
    
    for (unsigned int j = 0; j < gfs.size(); ++j) {
        std::cout << gfs[j] -> beta << " ";
    }

    std::cout << HFactorFunction::gamma << std::endl;
}

long double norm(const std::vector<Real>&gradient) {
    Real sum = RealAddId;
    for (auto g: gradient) {
        sum += g * g;
    }
    return std::sqrt((long double)sum);
}

long double getMedian(const std::vector<Real>&xs) {
    std::vector<Real>copy(xs);
    std::sort(copy.begin(), copy.end());
    return (copy[copy.size() / 2] + copy[copy.size() / 2 - 1]) / 2;
}

void gradientAscend(unsigned int batchSize, long double rate, Real converge,
                    const FactorGraph&factorGraph,
                    const std::vector<GFactorFunction*>&gFactorFunctions) {
    std::vector<TargetFunction*>marginalProbs;
    for (Index i = 0; i < factorGraph.nVars; ++i) {
        marginalProbs.push_back(new MarginalProb(i));
    }    
    
    Real gradientNorm;
    do {
        // random sample batch of candidates
        std::shuffle (marginalProbs.begin(), marginalProbs.end(), gen3);   
        std::vector<TargetFunction*>batch;
        std::copy(marginalProbs.begin(), marginalProbs.begin() + batchSize,
                  std::back_inserter(batch));

        // Sample marginal probability of candidates
        GibbsSampler marginSampler(batch, factorGraph);
        std::cout << "start sampling marginal" << std::endl;
        auto probs = marginSampler.doSample(16, 1, 100);

        // Start to spliting higher/lower        
        auto median = getMedian(probs);
        std::cout << "median:" << median << std::endl;
        
        std::vector<Index>hVarInds, lVarInds;
        Real phSum = RealAddId + 1;
        Real plSum = RealAddId + 1;
        for (Index i = 0; i < probs.size(); ++i) {
            if (probs[i] >= median && hVarInds.size() < probs.size() / 2) {
                phSum += probs[i];
                hVarInds.push_back(((MarginalProb*) batch[i]) -> varInd);
            } else {
                plSum += probs[i];
                lVarInds.push_back(((MarginalProb*) batch[i]) -> varInd);
            }            
        }

        std::cout << "phSum = " <<  phSum
                  << " plSum = " << plSum << std::endl;
        std::cout << "phSum / plSum = " << phSum / plSum << std::endl;
        std::cout << "size of hVarInds = " << hVarInds.size() << std::endl;
        
        // Make vector of target function (S).
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
        GibbsSampler gradientSampler(factors, factorGraph);
        std::cout << "start sampling gradient" << std::endl;

        auto gradient = gradientSampler.doSample(16, 100, 10);
        updateWeights(rate, gFactorFunctions, gradient);
        gradientNorm = norm(gradient);

        std::cout << "|gradient| = " << gradientNorm << std::endl;
        printWeights(gFactorFunctions);
    } while (gradientNorm > converge);
    
}
