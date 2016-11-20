#include "type.hpp"
#include "factor-graph.hpp"
#include <vector>

#ifndef TARGET_FUNCTIONS_HPP
#define TARGET_FUNCTIONS_HPP

class TargetFunction {
public:
    virtual Real eval(const EvalGraph&graph) const = 0;
};


class MarginalProb: public TargetFunction{
public:    
    Index varInd;
    MarginalProb(Index varInd);
    Real eval(const EvalGraph&graph) const;
};


class ExpectFactorF: public TargetFunction{
private:
    Index featureInd;
    const std::vector<Index>&varHInds;
    const std::vector<Index>&varLInds;
    const Real phSum, plSum;
public:    
    ExpectFactorF(Index featureInd,Real phSum, Real plSum,
                  const std::vector<Index>&varHInds,
                  const std::vector<Index>&varLInds);    
    Real eval(const EvalGraph&graph) const;
};


class ExpectFactorG: public TargetFunction{
private:
    GFactorFunction*function;
    const std::vector<Index>&varHInds;
    const std::vector<Index>&varLInds;
    const Real phSum, plSum;
public:    
    ExpectFactorG(GFactorFunction*function,Real phSum, Real plSum,
                  const std::vector<Index>&varHInds,
                  const std::vector<Index>&varLInds);    
    Real eval(const EvalGraph&graph) const;
};


class ExpectFactorH: public TargetFunction{
private:
    const std::vector<Index>&varHInds;
    const std::vector<Index>&varLInds;
    const Real phSum, plSum;
public:    
    ExpectFactorH(Real phSum, Real plSum,
                  const std::vector<Index>&varHInds,
                  const std::vector<Index>&varLInds);    
    Real eval(const EvalGraph&graph) const;
};



#endif
