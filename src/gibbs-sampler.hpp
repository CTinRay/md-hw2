#include "type.hpp"
#include "factor-graph.hpp"
#include "sample-target.hpp"
#include <vector>

class GibbsSampler {
private:
    SampleTarget&sampleTarget;
    const FactorGraph&factorGraph;
public:
    GibbsSampler(SampleTarget&sampleTarget,
                 const FactorGraph&factorGraph);
    void doSample(unsigned int nChains,
                  unsigned int nIgnore,
                  long double convergeRatio);
};

