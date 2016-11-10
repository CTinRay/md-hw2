#include "type.hpp"
#include "factor-functions.hpp"
#include <vector>
#include <cmath>

#define EXP expl


FFactorFunction::FFactorFunction(const std::vector<Real>&features)
    :features(features){}

Real FFactorFunction::eval(const std::vector<Label>&args) const{
    return EXP(args[0] * (alpha[0] * features[0] +
                           alpha[1] * features[1] +
                           alpha[2] * features[2] ) );
}


GFactorFunction::GFactorFunction(Real beta)
    :beta(beta){}

Real GFactorFunction::eval(const std::vector<Label>&args) const{
    return EXP(beta * args[0] * args[1]);
}


HFactorFunction::HFactorFunction(unsigned int ti)
    :ti(ti){};

Real HFactorFunction::eval(const std::vector<Label>&args) const{
    unsigned int count = 0;
    for (unsigned int i = 0; i < args.size(); ++i ){
        count += args[i] == 1 ? 1 : 0;
    }
    return EXP(gamma * (ti - count) * (ti - count));
}
