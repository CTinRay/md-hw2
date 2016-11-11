#include "../factor-functions.hpp"
#include "../gibbs-sampler.hpp"
#include <iostream>
#include <cmath>


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

class MarginalProb2: public TargetFunction{
private:
    Index varInd1, varInd2;
public:    
    MarginalProb2(Index varInd1, Index varInd2)
        :varInd1(varInd1),
         varInd2(varInd2){}

    Real eval(const std::vector<Label>&sample) const{
        return (sample[varInd1] == Label::positive &&
                sample[varInd2] == Label::positive) ? 1 : 0;
    }
};


class TestFactorFunction1: public FactorFunction{
public:
    TestFactorFunction1(){
    }
    
    Real eval(const std::vector<Label>&args) const {
        if (args[0] == Label::positive) {
            return 3;
        }
        else {
            return 7;
        }
    }
};



class MatrixFactorFunction: public FactorFunction{
private:
    std::vector<Real>matrix;
public:    
    MatrixFactorFunction(std::vector<Real>&matrix)
        :matrix(matrix){
    }
    
    Real eval(const std::vector<Label>&args) const {
        Index ind = 0;        
        for (unsigned int i = 0; i < args.size(); ++i) {
            ind += pow(2, i) * (args[i] == positive ? 1 : 0);
        }
        return matrix[ind];
    }
};


bool test1(){
    FactorGraph graph(3);
    std::vector<TargetFunction*>funcs;    
    funcs.push_back(new MarginalProb(0));
    funcs.push_back(new MarginalProb(1));
    for (int i = 0; i < 10; ++i ){
        GibbsSampler sampler(funcs, graph);
        auto probs = sampler.doSample(16, 10, 1.0001);
        if (std::abs(probs[0] - 0.5) > 0.01 || abs(probs[1] - 0.5) > 0.05) {
            std::cout << "(X) Fail test1: prob "
                      << probs[0] << " "
                      << probs[1] << std::endl;
            return false;
        }
    }
    std::cout << "(O) Pass test1" << std::endl;
    return true;
}



bool test2(){
    FactorGraph graph(5);
    TestFactorFunction1 f;
    std::vector<Index>scope(2);
    scope[0] = 2;
    scope[1] = 0;
    graph.addFactor(scope, f);
    std::vector<TargetFunction*>funcs;    
    funcs.push_back(new MarginalProb(2));
    funcs.push_back(new MarginalProb(0));
    funcs.push_back(new MarginalProb2(2, 3));    
    for (int i = 0; i < 10; ++i ){
        GibbsSampler sampler(funcs, graph);
        auto probs = sampler.doSample(8, 10, 1.0001);
        if (std::abs(probs[0] - 0.3) > 0.01 ||
            std::abs(probs[1] - 0.5) > 0.01 ||
            std::abs(probs[2] - 0.15) > 0.01) {
            std::cout << "(X) Fail test2" << std::endl;
            return false;
        }
    }
    std::cout << "(O) Pass test2" << std::endl;
    return true;
}

bool test3(){
    FactorGraph graph(5);
    std::vector<Real>matrix(4);
    matrix[0] = 1;
    matrix[1] = 2;
    matrix[2] = 3;
    matrix[3] = 4;
    MatrixFactorFunction f(matrix);
    std::vector<Index>scope(2);
    scope[0] = 0;
    scope[1] = 1;
    graph.addFactor(scope, f);
    scope[0] = 1;
    scope[1] = 2;
    graph.addFactor(scope, f);
    scope[0] = 2;
    scope[1] = 0;
    graph.addFactor(scope, f);

    std::vector<TargetFunction*>funcs;    
    funcs.push_back(new MarginalProb(0));
    for (int i = 0; i < 10; ++i ){
        GibbsSampler sampler(funcs, graph);
        auto probs = sampler.doSample(16, 100, 1.0001);
        if (std::abs(probs[0] - (118.0 / 154.0)) > 0.01 ) {
            std::cout << "(X) Fail test3: prob" << probs[0] << std::endl;
            return false;
        }
    }
    std::cout << "(O) Pass test3" << std::endl;
    return true;
    
}

int main(){
    // std::vector<Index>args(1, 0);
    // TestFactorFunction t;
    // std::vector<TestFactorFunction>ts;
    // graph.addFactor(args, t);
    test1();
    test2();
    test3();
}
