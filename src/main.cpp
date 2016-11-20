#include "factor-graph.hpp"
#include "factor-functions.hpp"
#include "construct.hpp"
#include "gradient.hpp"
#include "type.hpp"
#include "boost/program_options.hpp"
#include <iostream>
#include <string>
#include <cmath>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    
    std::string directory;
    int maxDistance;
    unsigned int batchSize;
    long double rate;
    Real converge;
    Real alpha, beta, gamma;

    try {
        
        po::options_description required("required arguments");
        required.add_options()
            ("directory,d", po::value<std::string>(&directory)->required(), "directory of the data set")
        ;

        po::options_description optional("optional arguments");
        optional.add_options()
            ("help,h", "Print help messages")
            ("maxDistance", po::value<int>(&maxDistance)->default_value(3),
             "max distance of users relationship for BFS")
            ("batchSize", po::value<unsigned int>(&batchSize)->default_value(1000), 
             "batch size for gradient ascent")
            ("rate", po::value<long double>(&rate)->default_value(0.1, "0.1"), 
             "learning rate for gradient ascent")
            ("converge", po::value<Real>(&converge)->default_value(0.1, "0.1"), 
             "converge condition for gradient ascent")
            ("alpha", po::value<Real>(&alpha)->default_value(0.01, "0.01"),
             "initial value of alpha")
            ("beta", po::value<Real>(&beta)->default_value(0.01, "0.01"),
             "initial value of beta")
            ("gamma", po::value<Real>(&gamma)->default_value(-0.01, "-0.01"),
             "initial value of gamma")
        ;

        po::options_description desc("Allowed options");
        desc.add(required).add(optional);
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }
        
        po::notify(vm);
    
    }
    catch (boost::program_options::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        return 0;
    }
    
    if (directory.back() != '/') {
        directory += '/';
    }
    
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
