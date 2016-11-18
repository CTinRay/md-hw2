#include "factor-graph.hpp"
#include "target-functions.hpp"
#include "gibbs-sampler.hpp"
#include <vector>

#ifndef GRADIENT_HPP
#define GRADIENT_HPP

void updateWeights(long double rate, const std::vector<GFactorFunction*>&gfs, const std::vector<Real>&gradient);
long double norm(const std::vector<Real>&gradient);
void gradientAscend(unsigned int batchSize, long double rate, Real converge, const FactorGraph&factorGraph);

#endif
