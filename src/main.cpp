#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include "construct2.hpp"

#define BATCH_SIZE 10
#define NUM_ITER_TO_START 500
bool vectorHas(const std::vector<Index> kMaxIndexArr, Index user){
    for(auto i: kMaxIndexArr){
        if(i == user){
            return true;
        }
    }
    return false;
}
void recordMax(Real potential, Index user, std::vector<Real>& kMax, std::vector<Index>& kMaxIndexArr){
    if(potential < kMax[0]){
        return;
    }
    Index insert = kMax.size() - 1;
    for(auto i = 0u; i < kMax.size(); i++){
        if(potential < kMax[i]){
            insert = i-1;
            break;
        }
    }
    for(auto i = 0u; i < insert; i++){
        kMax[i] = kMax[i+1];
        kMaxIndexArr[i] = kMaxIndexArr[i+1];
    }
    kMax[insert] = potential;
    kMaxIndexArr[insert] = user;
}
Real computeLogProb(ConstructGraph& graph, std::vector<Index>& kMaxIndexArr, Index item){
    Real logProb = 0;
    for(Index u = 0; u < graph.userNum; u++){
        Real p, z;
        if(vectorHas(kMaxIndexArr, u)){
            p = graph.FPotentialFunc(u, item, 1);
        }else{
            p = graph.FPotentialFunc(u, item, -1);
        }
        z = p + 1/p;
        logProb += (log(p)-log(z));
    }
    return logProb;
}
void predResult(ConstructGraph& graph, std::string& outFilename){
//    std::ofstream outStream(outFilename); 
//    for(Index i = 0; i < graph.taskNum; i++){
//        Index user = graph.taskUser[i], item = graph.taskItem[i];
//        Real potential = graph.FPotentialFunc(user, item, 1);
//        Real z = potential + 1/potential;
//        if(potential/z > 0.5){
//            outStream << user << " " << item << " " << 1 << std::endl;
//        }else{
//            outStream << user << " " << item << " " << 0 << std::endl;
//        }
//    }
//    outStream.close();
    std::ofstream weightStream(outFilename); 
    for(Index i = 0; i < 3; i++){
        for(Index j = 0; j < WEIGHT_NUM; j++){
            weightStream << graph.weightArr[i][j] << " ";
        }
        weightStream << std::endl;
    }
}
void fillItem2UserPred(ConstructGraph& graph, std::vector<std::set<Index>>& item2userPred){
    for(Index task = 0; task < graph.taskNum; task++){
        item2userPred[graph.taskItem[task]].insert(graph.taskUser[task]);
    }
}
int main(int argc, char* argv[]){
    if(argc < 6){
        std::cout << "wrong arguments" << std::endl;
    }
    Real learningRate = 0.000001;
    std::string userFilename(argv[1]), relationFilename(argv[2]), messageFilename(argv[3]);
    std::string predFilename(argv[4]), outFilename(argv[5]);

    ConstructGraph graph;
    graph.insertData(userFilename, relationFilename, messageFilename, predFilename);
    if(argc == 7){
        std::ifstream initWeightStream(argv[6]); 
        for(auto k = 0u; k < 3; k++){
            for(auto l = 0u; l < WEIGHT_NUM; l++){
                initWeightStream >> graph.weightArr[k][l];
                std::cout << graph.weightArr[k][l] << std::endl;
            }
        }
        initWeightStream.close();
    }

    std::cout << "user: " << graph.userNum << std::endl;
    std::cout << "item: " << graph.itemNum << std::endl;

    std::vector<std::set<Index>> item2userPred(graph.itemNum);
    fillItem2UserPred(graph, item2userPred);
    unsigned int count = 0;
    bool predAllTrue = true;
    for(Index n = 0; n < graph.itemNum; n += BATCH_SIZE){
        unsigned int batchSize = (graph.itemNum-n < BATCH_SIZE)? graph.itemNum-n:BATCH_SIZE;
        Real totalProb = 0;
        std::vector<std::vector<Real>> gredient(3, std::vector<Real>(WEIGHT_NUM, 0));
        for(Index i = n; i < n + batchSize; i++){
            predAllTrue = (i > NUM_ITER_TO_START)? false:true;
            std::vector<Real> kMax(graph.itemLinkCount[i], 0);
            std::vector<Index> kMaxIndexArr(graph.itemLinkCount[i]);
            //std::vector<Real> userProb(graph.userNum);
//            std::cout << "logProb: " << computeLogProb(graph, kMaxIndexArr, i) << std::endl;

            //Real potential;
            if(graph.itemLinkCount[i] != 0){
                for(Index u = 0; u < graph.userNum; u++){
                    recordMax(graph.FPotentialFunc(u, i, 1), u, kMax, kMaxIndexArr);
                }
//                std::cout << "h: " << kMax[kMax.size()-1] << std::setw(15) << std::right << " l: " << kMax[0] << ' '<< kMax.size() << std::endl;
            }
            int assign;
            for(Index u = 0; u < graph.userNum; u++){
                std::vector<std::vector<Real>> f = graph.getFeatures(u, i);
                Real p = graph.FPotentialFunc(u, i, 1);
                p = p * p;
                if(vectorHas(kMaxIndexArr, u) || (predAllTrue && item2userPred[i].find(u) != item2userPred[i].end())){
                    assign = 1;
                }else{
                    assign = -1;
                }
                for(auto k = 0u; k < f.size(); k++){
                    for(auto l = 0u; l < f[k].size(); l++){
                        //std::cout << "f " <<k << ' ' <<l<< ' :'<< std::setw(15) << std::left << f[k][l] << std::endl;
                        gredient[k][l] += f[k][l] * (assign - (p - 1)/(p + 1));
                    }
                }
                count++;
            }

            for(auto l = 0u; l < WEIGHT_NUM; l++){
                if(graph.weightArr[0][l] * graph.weightArr[1][l] < 0){
                    Real d = graph.weightArr[0][l] + graph.weightArr[1][l];
                    if(graph.weightArr[0][l] * d > 0){
                        graph.weightArr[0][l] = d;
                        graph.weightArr[1][l] = 0;
                    }else{
                        graph.weightArr[0][l] = 0;
                        graph.weightArr[1][l] = d;
                    }
                }
            }
            Real prob = computeLogProb(graph, kMaxIndexArr, i);
            totalProb += prob;

//            std::cout << "after: " << prob << std::endl << std::endl;
        }
        for(auto k = 0u; k < 3; k++){
            for(auto l = 0u; l < WEIGHT_NUM; l++){
                std::cout << std::setw(15) << std::left << graph.weightArr[k][l] << " " << std::setw(15) << std::left << gredient[k][l] * learningRate;
                graph.weightArr[k][l] += gredient[k][l] * learningRate;
                std::cout << " " << std::setw(15) << std::left << graph.weightArr[k][l] << std::endl;
            }
        }
        std::cout << n << " batch: " << totalProb << std::endl << std::endl;
        predResult(graph, outFilename);
    }
    std::cout << "user: " << graph.userNum << std::endl;
    std::cout << "item: " << graph.itemNum << std::endl;
    std::cout << "count: " << count << std::endl;
    return 0;
}
