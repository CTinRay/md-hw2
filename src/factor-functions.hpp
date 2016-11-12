#include "type.hpp"
#include <vector>

#ifndef FACTOR_FUNCTIONS_HPP
#define FACTOR_FUNCTIONS_HPP

// Virtual class that defines interface of FactorFunctions
class FactorFunction{
public:
    virtual Real eval(const std::vector<std::vector<Label>::iterator>&args)
        const = 0;
};


// Factor function in the bottom layer of the three-layer graph in paper.
// 
// Each factor object shares the same parameters vector "alpha". Therefore,
// as we update alpha, all instance of this class will be updated.
// Also remember to initialize alpha before eval is called.
//
// This class of factor function (instance of this class) acts as prior of
// candidates according to their features. For example, for a candidate
// y = (u, i), it has feature like distance(u, O(i)). Feature of candidate
// affects how possible it is +1. Therefore, we use feature to construct
// different instance of factor function for each candidate.
class FFactorFunction: public FactorFunction{
public:
    static std::vector<Real>alpha;
    const std::vector<Real>features;
    FFactorFunction(const std::vector<Real>&features);
    Real eval(const std::vector<std::vector<Label>::iterator>&args) const;    
};

// Factor function in the middle layer of the three-layer graph in paper.
//
// Factors in this layer has exactly the same form, except their beta.
// Specifically, there are only three different beta. Therefore, only
// three instances are needed to be construct.
class GFactorFunction: public FactorFunction{
public:
    Real beta;
    GFactorFunction(Real beta);
    Real eval(const std::vector<std::vector<Label>::iterator>&args) const;    
};

// Factor function in the top layer of the three-layer graph in paper.
class HFactorFunction: virtual FactorFunction{
    static Real gamma;
    unsigned int ti;
    HFactorFunction(unsigned int ti);
    Real eval(const std::vector<std::vector<Label>::iterator>&args) const;    
};

#endif
