#include "gibbs-sampler.hpp"
#include <random>
#include <thread>
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
    :evalGraph(factorGraph),
     targetFunctions(targetFunctions),
     nItered(0),
     sum(std::vector<Real>(targetFunctions.size(), RealAddId)),
     squareSum(std::vector<Real>(targetFunctions.size(), RealAddId)),
     means(std::vector<Real>(targetFunctions.size())),
     vars(std::vector<Real>(targetFunctions.size()))
{
    srand(time(NULL));
    for (auto i = 0u; i < evalGraph.assignment.size(); ++i){
        evalGraph.assignment[i] = rand(1, 1);
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
            Real res = targetFunctions[j] -> eval(evalGraph);
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
    for (auto i = 0u; i < evalGraph.assignment.size(); ++i ){
        evalGraph.assignment[i] = Label::positive;
        Real potentialP = evalGraph.evalAt(i);
        evalGraph.assignment[i] = Label::negative;
        Real potentialN = evalGraph.evalAt(i);
        evalGraph.assignment[i] = rand(potentialP, potentialN);
    }
}


GibbsSampler::GibbsSampler(const std::vector<TargetFunction*>&targetFunctions,
                           const FactorGraph&factorGraph)
    :targetFunctions(targetFunctions),
     factorGraph(factorGraph){}


bool GibbsSampler::isConverge(long double convergeRatio,
                              const std::vector<GibbsSampler::Chain>&chains){
    std::vector<Real>interChainMeans(targetFunctions.size(), 0);
    std::vector<Real>interChainVars(targetFunctions.size(), 0);
    std::vector<Real>intraChainVars(targetFunctions.size());

    // mean of mean of chains
    for (auto&&chain: chains) {
        const auto&means = chain.getMeans();
        for (auto i = 0u; i < means.size(); ++i) {
            interChainMeans[i] += means[i] / chains.size();
        }
    }

    for (auto&&chain: chains) {
        const auto&means = chain.getMeans();
        const auto&vars = chain.getVars();
        // variance of mean of chains
        for (auto i = 0u; i < means.size(); ++i) {
            interChainVars[i] += SQUARE(means[i] - interChainMeans[i]) / chains.size();
        }
        // average of variance of chains
        for (auto i = 0u; i < means.size(); ++i) {
            intraChainVars[i] += vars[i] / chains.size();
        }        
    }

    convergeRatio = SQUARE(convergeRatio);
    for (auto i = 0u; i < intraChainVars.size(); ++i) {
        if (interChainVars[i] / intraChainVars[i] > convergeRatio) {
            // std::cout << "interchain " << i << " var = " << interChainVars[i] << std::endl;
            // std::cout << "intrachain " << i << " var = " << intraChainVars[i] << std::endl;

            for (auto&&chain: chains) {
                std::cout << "mean i" << chain.getMeans()[i] << std::endl;
            }
            
            return false;
        }            
    }
    return true;
}
    
std::vector<Real> GibbsSampler::doSample(unsigned int nChains,
                                                unsigned int nIgnore,
                                                long double convergeRatio){
    std::vector<Chain>chains(nChains, Chain(factorGraph, targetFunctions));
    std::vector<std::thread>threads(chains.size());

    // burn-in step    
    for (auto i = 0u; i < chains.size(); ++i) {
        threads[i] = std::thread(&Chain::init, &chains[i], nIgnore);
    }
    for (auto&&thread: threads) {
        thread.join();
    }
    
    // start sampling
    // for (auto&& chain: chains){
    //     chain.iterate(10);
    // }


    do {
        // std::cout << "100 iter start" << std::endl;
        for (auto i = 0u; i < chains.size(); ++i) {
            threads[i] = std::thread(&Chain::iterate, &chains[i], 100);
        }
        for (auto&&thread: threads) {
            thread.join();
        }
        // std::cout << "100 iter end" << std::endl;
        
    } while (!isConverge(convergeRatio, chains));


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
