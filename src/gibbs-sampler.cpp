#include "gibbs-sampler.hpp"
#include <random>

std::random_device rd;
std::mt19937_64 gen(rd());    


Label rand(Real potenrialP, Real potenrialN){
    std::bernoulli_distribution d(potenrialP / (potenrialN + potenrialP));
    return d(gen) ? Label::positive : Label::negative;
}

GibbsSampler::Chain::Chain(const FactorGraph&factorGraph,
                           const std::vector<TargetFunction>&targetFunctions)
    :factorGraph(factorGraph),
     targetFunctions(targetFunctions),
     assignment(std::vector<Label>(factorGraph.nVars)),
     means(std::vector<Real>(targetFunctions.size(), RealAddId)),
     vars(std::vector<Real>(targetFunctions.size(), RealAddId)),
     results(targetFunctions.size(), std::vector<Real>())
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
            results[j].push_back(targetFunctions[j].eval(assignment));
        }
    }
}


void GibbsSampler::Chain::step(){
    for (auto i = 0u; i < factorGraph.nVars; ++i ){
        assignment[i] = Label::positive;
        Real potentialP = factorGraph.evalAt(i, assignment);
        assignment[i] = Label::negative;
        Real potentialN = factorGraph.evalAt(i, assignment);
        assignment[i] = rand(potentialP, potentialN);
    }
}


GibbsSampler::GibbsSampler(const std::vector<TargetFunction>&targetFunctions,
                           const FactorGraph&factorGraph)
    :targetFunctions(targetFunctions),
     factorGraph(factorGraph){}


std::vector<Real> GibbsSampler::doSample(unsigned int nChains,
                                                unsigned int nIgnore,
                                                long double convergeRatio){
    std::vector<Chain>chains(nChains, Chain(factorGraph, targetFunctions));

    // burn-in step
    for (auto&& chain: chains) {
        chain.init(nIgnore);
    }

    // start sampling
    while (!isConverge(convergeRatio, chains)){
        for (auto&& chain: chains){
            chain.iterate(1);
        }
    }

    // average for result
    std::vector<Real>means(targetFunctions.size());
    for (auto&& chain: chains) {
        auto&m = chain.getMeans();
        for (auto i = 0u; i < means.size(); ++i) {
            means[i] += m[i];
        }
    }
    for (auto i = 0u; i < means.size(); ++i) {
        means[i] /= nChains;
    }
                                                
    return means;
}
