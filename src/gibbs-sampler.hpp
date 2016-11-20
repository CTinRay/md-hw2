#include "factor-graph.hpp"
#include "target-functions.hpp"
#include <random>

#ifndef GIBBS_SAMPLER_HPP
#define GIBBS_SAMPLER_HPP

class GibbsSampler {
private:
    const std::vector<TargetFunction*>&targetFunctions;
    const FactorGraph&factorGraph;
    class Chain{
    private:
        std::mt19937_64 gen;    
        EvalGraph evalGraph;
        const std::vector<TargetFunction*>&targetFunctions;
        unsigned int nItered;
        std::vector<Real>sum, squareSum, means, vars;
        void step();
        void updateMeanVar();
        Label rand(Real potenrialP, Real potenrialN);
    public:
        Chain(const FactorGraph&factorGraph,
              const std::vector<TargetFunction*>&targetFunctions);
        void init(unsigned int nIgnore);
        void iterate(unsigned int nIter);
        const std::vector<Real>&getMeans() const;
        const std::vector<Real>&getVars() const;
    };
    bool isConverge(long double ratio,
                    const std::vector<Chain>&chains);
public:
    GibbsSampler(const std::vector<TargetFunction*>&targetFunctions,
                 const FactorGraph&factorGraph);
    std::vector<Real> doSample(unsigned int nChains,
                               unsigned int nIgnore,
                               long double convergeRatio);
};


#endif
