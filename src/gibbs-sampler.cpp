#include "gibbs-sampler.hpp"

GibbsSampler::GibbsSampler(const std::vector<TargetFunction>&targetFunctions,
                           const FactorGraph&factorGraph)
    :targetFunctions(targetFunctions),
     factorGraph(factorGraph){}


std::vector<long double> GibbsSampler::doSample(unsigned int nChains,
                                                unsigned int nIgnore,
                                                long double convergeRatio){
    std::vector<Chain>chains(nChains);

    // burn-in step
    for (auto&& chain: chains) {
        chain.init(nIgnore, factorGraph);
    }

    // start sampling
    while (!isConverge(convergeRatio, chains)){
        for (auto&& chain: chains){
            chain.iterate(1, factorGraph, targetFunctions);
        }
    }

    // average for result
    std::vector<long double>means(targetFunctions.size());
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
