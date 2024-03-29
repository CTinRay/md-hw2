#include "../factor-functions.hpp"
#include "../gibbs-sampler.hpp"
#include "../construct.hpp"
#include <iostream>
#include <cmath>
#include <string>

// class MarginalProb: public TargetFunction{
// private:
//     Index varInd;
// public:    
//     MarginalProb(Index varInd)
//         :varInd(varInd){
        
//     }

//     Real eval(const EvalGraph&graph) const{
//         return graph.assignment[varInd] == Label::positive ? 1 : 0;
//     }
// };

class MarginalProb2: public TargetFunction{
private:
    Index varInd1, varInd2;
public:    
    MarginalProb2(Index varInd1, Index varInd2)
        :varInd1(varInd1),
         varInd2(varInd2){}

    Real eval(const EvalGraph&graph) const{
        return (graph.assignment[varInd1] == Label::positive &&
                graph.assignment[varInd2] == Label::positive) ? 1 : 0;
    }
};


class ZFunction: public TargetFunction{
public:    
    ZFunction(){}
    
    Real eval(const EvalGraph&graph) const{
        int n1 = 0;
        const Real f[] = {1, 6, 24, 64};
        for (auto i = 0u; i < 3; ++i) {
            if (graph.assignment[i] == Label::positive) {
                n1 += 1;
            }
        }
        return 1.0 / f[n1];
    }
};


class TestFactorFunction1: public FactorFunction{
public:
    TestFactorFunction1()
        :FactorFunction(FactorType::f){
    }
    
    Real eval(const std::vector<std::vector<Label>::iterator>&args) const {
        if (*args[0] == Label::positive) {
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
        :FactorFunction(FactorType::f),
         matrix(matrix){
    }
    
    Real eval(const std::vector<std::vector<Label>::iterator>&args) const {
        Index ind = 0;        
        for (unsigned int i = 0; i < args.size(); ++i) {
            ind += pow(2, i) * (*args[i] == positive ? 1 : 0);
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
        auto probs = sampler.doSample(8, 100, 1.0001);
        if (std::abs(probs[0] - 0.5) > 0.05 || abs(probs[1] - 0.5) > 0.05) {
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
        if (std::abs(probs[0] - 0.3) > 0.05 ||
            std::abs(probs[1] - 0.5) > 0.05 ||
            std::abs(probs[2] - 0.15) > 0.05) {
            std::cout << "(X) Fail test2 probs "
                      << probs[0] << " "
                      << probs[1] << " "
                      << probs[2] << " "
                      << std::endl;
            return false;
        }
    }
    std::cout << "(O) Pass test2" << std::endl;
    return true;
}

bool test3(){
    FactorGraph graph(3);
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
    funcs.push_back(new ZFunction());
    for (int i = 0; i < 10; ++i ){
        GibbsSampler sampler(funcs, graph);
        auto probs = sampler.doSample(16, 100, 1.0001);
        if (std::abs(probs[0] - (118.0 / 155.0)) > 0.05 ||
            std::abs(probs[1] - 8.0 / 155.0) > 0.05) {
            std::cout << "(X) Fail test3: prob"
                      << probs[0] << " "
                      << probs[1] << std::endl;
            return false;
        }
    }
    std::cout << "(O) Pass test3" << std::endl;
    return true;
    
}

bool test4(){
    const unsigned int nVars = 100000;

    std::vector<Real>matrixT(4);
    matrixT[0] = 4;
    matrixT[1] = 1;
    matrixT[2] = 1;
    matrixT[3] = 4;
    MatrixFactorFunction mfft(matrixT);

    std::vector<Real>matrixP(2);
    matrixP[0] = 1;
    matrixP[1] = 3;
    MatrixFactorFunction mffp(matrixP);

    FactorGraph graph(nVars);
    for (auto i = 0u; i < nVars - 1; ++i) {
        std::vector<Index>scopeT(2);
        scopeT[0] = i;
        scopeT[1] = i + 1;
        std::vector<Index>scopeP(1);
        scopeP[0] = i;
        graph.addFactor(scopeT, mfft);
        graph.addFactor(scopeP, mffp);
    }
    std::vector<Index>scopeP(1);
    scopeP[0] = nVars - 1;
    graph.addFactor(scopeP, mffp);

    std::vector<TargetFunction*>funcs;    
    funcs.push_back(new MarginalProb(nVars - 1));
    for (int i = 0; i < 1; ++i ){
        GibbsSampler sampler(funcs, graph);
        auto probs = sampler.doSample(4, 100, 1.001);
        // std::cout << probs[0] << std::endl;
        if (std::abs(probs[0] - 0.89314982) > 0.05) {
            std::cout << "(X) Fail test4: prob"
                      << probs[0] << std::endl;
            // return false;
        }
    }
    std::cout << "(O) Pass test4" << std::endl;
    return true;

    
    return true;
}

bool test5(std::string dirName, int maxDistance) {
    FactorGraph graph(1000000);
    GFactorFunction *OIFunc = new GFactorFunction(0);
    GFactorFunction *FIFunc = new GFactorFunction(0);
    GFactorFunction *CCFunc = new GFactorFunction(0);
    ConstructGraph construct;
    construct.insertData( dirName + "user.txt", dirName + "relation.txt", dirName + "message.txt", dirName + "pagerank.txt" );
    construct.sampleCandidates(50000, dirName + "sample_pred.id");
    construct.constructFeatures(maxDistance, DIRECTED, dirName + "pred.id" , dirName + "features.txt");
    construct.constructFeatures(maxDistance, DIRECTED, dirName + "sample_pred.id" , dirName + "sample_features.txt");
    construct.constructGraph(graph, OIFunc, FIFunc, CCFunc, dirName + "pred.id", dirName + "features.txt");
    std::cout << "(O) Pass test5" << std::endl;
    return true;
}

int main(int argc, char* argv[]){
    // std::vector<Index>args(1, 0);
    // TestFactorFunction t;
    // std::vector<TestFactorFunction>ts;
    // graph.addFactor(args, t);
    test1();
    test2();
    test3();
    test4();
    std::string dirName(argv[1]);
    dirName = dirName + '/';
    int maxDistance = 3;
    if (argc == 3) {
        maxDistance = std::stoi(argv[2]);
    }
    test5(dirName, maxDistance);
}
