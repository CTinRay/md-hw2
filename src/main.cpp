#include "factor-graph.hpp"
#include "factor-functions.hpp"
#include "construct.hpp"
#include "gradient.hpp"
#include "type.hpp"
#include "boost/program_options.hpp"
#include <iostream>
#include <string>
#include <cassert>

// namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    
    std::string directory(argv[1]);
    directory += '/';
    int maxDistance = 3;
    unsigned int batchSize = 1000;
    long double rate = 0.01;
    Real converge = 0.1;
    Real alpha = 0.01, beta = 0.01, gamma = -0.01;

    /*try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "Print help messages")
            ("directory,d", po::value<std::string>(&directory)->required(), "directory of the data set")
            ("maxDistance", po::value<int>(&maxDistance)->default_value(3),
             "max distance of users relationship for BFS (3)")
            ("batchSize", po::value<unsigned int>(&batchSize)->default_value(1000), 
             "batch size for gradient ascent (1000)")
            ("rate", po::value<long double>(&rate)->default_value(0.1), 
             "learning rate for gradient ascent (0.1)")
            ("converge", po::value<Real>(&converge)->default_value(0.1), 
             "converge condition for gradient ascent (0.1)")
            ("alpha", po::value<Real>(&alpha)->default_value(0.01),
             "initial value of alpha")
            ("beta", po::value<Real>(&beta)->default_value(0.01),
             "initial value of beta")
            ("gamma", po::value<Real>(&gamma)->default_value(-0.01),
             "initial value of gamma")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (boost::program_options::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    }*/
    
    FactorGraph factorGraph(82318);
    FFactorFunction::alpha.resize(5, alpha);
    HFactorFunction::gamma = gamma;
    GFactorFunction *OIFunc = new GFactorFunction(beta);
    GFactorFunction *FIFunc = new GFactorFunction(beta);
    GFactorFunction *CCFunc = new GFactorFunction(beta);

    std::vector<GFactorFunction*>gFactorFunctions;
    gFactorFunctions.push_back(OIFunc);
    gFactorFunctions.push_back(FIFunc);
    gFactorFunctions.push_back(CCFunc);
    
    ConstructGraph construct;
    construct.insertData( directory + "user.txt", directory + "relation.txt", directory + "message.txt", directory + "pagerank.txt" );
    construct.constructFeatures(maxDistance, DIRECTED, directory + "pred.id" , directory + "features.txt");
    construct.constructGraph(factorGraph, OIFunc, FIFunc, CCFunc, directory + "pred.id", directory + "features.txt");
    
    gradientAscend(batchSize, rate, converge, factorGraph, gFactorFunctions);

    return 0;
}
