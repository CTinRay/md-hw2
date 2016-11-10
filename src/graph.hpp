#include <vector>

typedef long double Real;
typedef short Label;
typedef unsigned int Index;

class FactorFunction{
public:
    virtual Real eval(const std::vector<Label>&args) const;
};

class FactorGraph{
public:
    unsigned int nVars;
    FactorGraph(unsigned int nVars);
    std::vector<Index>varFactors;
    void addFactor(const std::vector<unsigned int>&scope,
                   const FactorFunction&factorFunction);
                      

};


class GFactorFunction: FactorFunction{
public:
    Real beta;
    GFactorFunction(Real beta);
    Real eval(const std::vector<Label>&args) const{
        return expl(beta * args[0] * args[1]);
    }
};


class FFactorFunction: FactorFunction{
    static std::vector<Real>alpha;
    std::vector<Real>&features;
    FFactorFunction(const std::vector<Real>&features);
    Real eval(const std::vector<Label>&args) const{
        return expl(args[0] * (alpha[0] * features[0] +
                               alpha[1] * features[1] +
                               alpha[2] * features[2] ) );
    }
};



class HFactorFunction: FactorFunction{
    static Real gamma;
    unsigned int ti;
    std::vector<Real>&features;
    HFactorFunction(unsigned int ti);
    Real eval(const std::vector<Label>&args) const{
        unsigned int count = 0;
        for (unsigned int i = 0; i < args.size(); ++i ){
            count += args[i] == 1 ? 1 : 0;
        }
        return expl(gamma * (ti - count) * (ti - count));
    }
};

int funct(){
    CCFactorFunction* OI = new CCFactorFunction(1);
    CCFactorFunction* FI = new CCFactorFunction(1);
    CCFactorFunction* OF = new CCFactorFunction(1);
    
}
