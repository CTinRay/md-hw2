#include "factor-graph.hpp"
#include "factor-functions.hpp"
#include "construct.hpp"
#include "gradient.hpp"
#include "type.hpp"
#include "boost/program_options.hpp"
#include "target-functions.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>

// namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    
    std::string directory(argv[1]);
    directory += '/';
    int maxDistance = 3;
    // unsigned int batchSize = 1000;
    // long double rate = 0.01;
    // Real converge = 0.1;
    // Real alpha = 0.01, beta = 0.01, gamma = -0.01;
    FactorGraph factorGraph(800000);
    FFactorFunction::alpha.resize(5);
    FFactorFunction::alpha[0] = 0.887841;
    FFactorFunction::alpha[1] = 0.999943;
    FFactorFunction::alpha[2] = -8.6576 ;
    FFactorFunction::alpha[3] = -10.4439;
    FFactorFunction::alpha[4] = -0.687381;        
    GFactorFunction *OIFunc = new GFactorFunction(-0.0201547);
    GFactorFunction *FIFunc = new GFactorFunction(0.00509222);
    GFactorFunction *CCFunc = new GFactorFunction(-1.08189);
    HFactorFunction::gamma = -9.15046;

    std::vector<GFactorFunction*>gFactorFunctions;
    gFactorFunctions.push_back(OIFunc);
    gFactorFunctions.push_back(FIFunc);
    gFactorFunctions.push_back(CCFunc);
    
    ConstructGraph construct;
    construct.insertData( directory + "user.txt", directory + "relation.txt", directory + "message.txt", directory + "pagerank.txt" );
    construct.constructFeatures(maxDistance, DIRECTED, directory + "pred.id" , directory + "features.txt");
    construct.constructGraph(factorGraph, OIFunc, FIFunc, CCFunc, directory + "pred.id", directory + "features.txt");
    
    // gradientAscend(batchSize, rate, converge, factorGraph, gFactorFunctions);

    std::vector<TargetFunction*>marginalProbs;
    // marginalProbs.push_back(new MarginalProb(0));
    for (Index i = 0; i < factorGraph.nVars; ++i) {
        marginalProbs.push_back(new MarginalProb(i));
    }    

    GibbsSampler marginSampler(marginalProbs, factorGraph);
    auto probs = marginSampler.doSample(16, 1000, 2);

    std::fstream f;    
    f.open(directory + "predict-probs.txt", std::fstream::out);
    for (auto p : probs) {
        f << p << std::endl;
    }
    f.close();
    
    return 0;
}
