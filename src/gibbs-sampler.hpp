#include "factor-graph.hpp"
#include "target-functions.hpp"

class GibbsSampler {
private:
    const std::vector<TargetFunction>&targetFunctions;
    const FactorGraph&factorGraph;
    class Chain{
    private:
        unsigned int nItered;
        void step(Index varIndex, std::vector<Label>&chain);
        std::vector<long double>means;
        std::vector<long double>vars;
    public:
        Chain();
        void init(unsigned int nIgnore,
                  const FactorGraph&factorGraph);
        void iterate(unsigned int nIter,
                     const FactorGraph&factorGraph,
                     const std::vector<TargetFunction>&targetFunctions);
        const std::vector<long double>getMeans();
        const std::vector<long double>getVars();
    };
    bool isConverge(long double ratio,
                       const std::vector<Chain>&chains);
public:
    GibbsSampler(const std::vector<TargetFunction>&targetFunctions,
                 const FactorGraph&factorGraph);
    std::vector<long double> doSample(unsigned int nChains,
                               unsigned int nIgnore,
                               long double convergeRatio);
};

