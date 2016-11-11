#include "../factor-functions.hpp"
#include "../gibbs-sampler.hpp"
#include <iostream>

class MarginalProb: public TargetFunction{
private:
    Index varInd;
public:    
    MarginalProb(Index varInd)
        :varInd(varInd){
        
    }

    Real eval(const std::vector<Label>&sample) const{
        return sample[varInd] == Label::positive ? 1 : 0;
    }
};

class TestFactorFunction: public FactorFunction{
public:
    TestFactorFunction(){
    }
    
    Real eval(const std::vector<Label>&args) const {
        return 0;
    }

};

int main(){
    FactorGraph graph(2);
    // std::vector<Index>args(1, 0);
    // TestFactorFunction t;
    // std::vector<TestFactorFunction>ts;
    // graph.addFactor(args, t); 
    std::vector<TargetFunction*>funcs;    
    funcs.push_back(new MarginalProb(0));
    GibbsSampler sampler(funcs, graph);
    std::cout << sampler.doSample(16, 10, 1.00001)[0]  << std::endl;
}
