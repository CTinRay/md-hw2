#include "gibbs-sampler.hpp"
#include <random>
#include <iostream>

#define SQUARE(X) (X) * (X)

std::random_device rd;
std::mt19937_64 gen(rd());    


Label rand(Real potenrialP, Real potenrialN){
    std::bernoulli_distribution d(potenrialP / (potenrialN + potenrialP));
    return d(gen) ? Label::positive : Label::negative;
}

GibbsSampler::Chain::Chain(const FactorGraph&factorGraph,
                           const std::vector<TargetFunction*>&targetFunctions)
    :factorGraph(factorGraph),
     targetFunctions(targetFunctions),
     nItered(0),
     assignment(std::vector<Label>(factorGraph.nVars)),
     sum(std::vector<Real>(targetFunctions.size(), RealAddId)),
     squareSum(std::vector<Real>(targetFunctions.size(), RealAddId)),
     means(std::vector<Real>(targetFunctions.size())),
     vars(std::vector<Real>(targetFunctions.size()))
{
    srand(time(NULL));
    for (auto i = 0u; i < assignment.size(); ++i){
        assignment[i] = (rand() & 1) == 0 ? Label::positive : Label::negative;
    }
}


void GibbsSampler::Chain::init(unsigned int nIgnore){
    for (auto i = 0u; i < nIgnore; ++i) {
        step();
    }
}


void GibbsSampler::Chain::iterate(unsigned int nIter){
    for (auto i = 0u; i < nIter; ++i) {
        step();
        for (auto j = 0u; j < targetFunctions.size(); ++j) {
            Real res = targetFunctions[j] -> eval(assignment);
            sum[j] += res;
            squareSum[j] += SQUARE(res);
        }
        nItered += 1;
    }
    updateMeanVar();
}


void GibbsSampler::Chain::updateMeanVar(){
    for (auto i = 0u; i < targetFunctions.size(); ++i) {
        means[i] = sum[i] / nItered;
        // Var = E(x^2) - (E(x))^2
        vars[i] = squareSum[i] / nItered - SQUARE(means[i]);
    }
}    


const std::vector<Real>&GibbsSampler::Chain::getMeans() const {
    return means;
}


const std::vector<Real>&GibbsSampler::Chain::getVars() const {
    return vars;
}


void GibbsSampler::Chain::step(){
    // std::cout << "step" << std::endl;
    for (auto i = 0u; i < factorGraph.nVars; ++i ){
        assignment[i] = Label::positive;
        Real potentialP = factorGraph.evalAt(i, assignment);
        assignment[i] = Label::negative;
        Real potentialN = factorGraph.evalAt(i, assignment);
        assignment[i] = rand(potentialP, potentialN);
    }
}


GibbsSampler::GibbsSampler(const std::vector<TargetFunction*>&targetFunctions,
                           const FactorGraph&factorGraph)
    :targetFunctions(targetFunctions),
     factorGraph(factorGraph){}


bool GibbsSampler::isConverge(long double convergeRatio,
                              const std::vector<GibbsSampler::Chain>&chains){
    std::vector<Real>interChainVars(targetFunctions.size());
    std::vector<Real>intraChainVars(targetFunctions.size());

    // mean of mean of chains
    for (auto&&chain: chains) {
        const auto&means = chain.getMeans();
        for (auto i = 0u; i < means.size(); ++i) {
            interChainVars[i] += means[i] / chains.size();
        }
    }

    for (auto&&chain: chains) {
        const auto&means = chain.getMeans();
        // variance of mean of chains
        for (auto i = 0u; i < means.size(); ++i) {
            interChainVars[i] += SQUARE(means[i] - interChainVars[i]) / chains.size();
        }
        // average of variance of chains
        for (auto i = 0u; i < means.size(); ++i) {
            intraChainVars[i] += means[i] / chains.size();
        }        
    }

    convergeRatio = SQUARE(convergeRatio);
    for (auto i = 0u; i < intraChainVars.size(); ++i) {
        if (interChainVars[i] / intraChainVars[i] > convergeRatio) {
            return false;
        }            
    }
    return true;
}
    
std::vector<Real> GibbsSampler::doSample(unsigned int nChains,
                                                unsigned int nIgnore,
                                                long double convergeRatio){
    std::vector<Chain>chains(nChains, Chain(factorGraph, targetFunctions));

    // burn-in step
    for (auto&& chain: chains) {
        chain.init(nIgnore);
    }

    // start sampling
    for (auto&& chain: chains){
        chain.iterate(10);
    }

    while (!isConverge(convergeRatio, chains)) {
        for (auto&& chain: chains){
            chain.iterate(1);
        }
    };


    // average for result
    std::vector<Real>means(targetFunctions.size());
    for (auto&& chain: chains) {
        auto&m = chain.getMeans();
        for (auto i = 0u; i < means.size(); ++i) {
            means[i] += m[i] / nChains;
        }
    }
                                                
    return means;
}
