#include <vector>

typedef long double Real;
typedef short Label;
typedef unsigned int Index;

class FactorFunction {
public:
    virtual Real eval(const std::vector<Label>&args) const;
};

class FactorGraph {
public:
    unsigned int nVars;
    FactorGraph(unsigned int nVars);
    std::vector<Index>varFactors;
    void addFactor(const std::vector<unsigned int>&scope,
                   const FactorFunction&factorFunction);
                      

};

