#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "construct2.hpp"

#define BATCH_SIZE 800
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
    std::ofstream outStream(outFilename); 
    std::vector<Real> pArr;
    for(Index i = 0; i < graph.taskNum; i++){
        Index user = graph.taskUser[i], item = graph.taskItem[i];
        Real potential = graph.FPotentialFunc(user, item, 1);
        pArr.push_back(potential);
//        Real z = potential + 1/potential;
        std::cout << user << " " << item << " " << potential << std::endl;
//        if(potential/z > 0.5){
//            outStream << user << " " << item << " " << 1 << std::endl;
//        }else{
//            outStream << user << " " << item << " " << 0 << std::endl;
//        }
    }
    std::sort(pArr.begin(), pArr.end());
    Real middle = pArr[pArr.size() / 2];
    std::cout << "middle " << middle << std::endl;
    for(Index i = 0; i < graph.taskNum; i++){
        Index user = graph.taskUser[i], item = graph.taskItem[i];
        Real potential = graph.FPotentialFunc(user, item, 1);
        if(potential > middle){
            outStream << user << " " << item << " " << 1 << std::endl;
        }else{
            outStream << user << " " << item << " " << 0 << std::endl;
        }
    }
    outStream.close();
}
int main(int argc, char* argv[]){
    if(argc < 7){
        std::cout << "wrong arguments" << std::endl;
        return 1;
    }
    Real learningRate = 0.0000001;
    std::string userFilename(argv[1]), relationFilename(argv[2]), messageFilename(argv[3]);
    std::string predFilename(argv[4]), outFilename(argv[5]), weightFilename(argv[6]);
    ConstructGraph graph;
    graph.insertData(userFilename, relationFilename, messageFilename, predFilename);
    std::cout << "user: " << graph.userNum << std::endl;
    std::cout << "item: " << graph.itemNum << std::endl;
    std::ifstream weightStream("weight.txt"); 
    for(auto k = 0u; k < 3; k++){
        for(auto l = 0u; l < WEIGHT_NUM; l++){
            weightStream >> graph.weightArr[k][l];
            std::cout << graph.weightArr[k][l] << std::endl;
        }
    }
    weightStream.close();
    predResult(graph, outFilename);
//    unsigned int count = 0;
//    for(Index n = 0; n < graph.itemNum; n += BATCH_SIZE){
//        unsigned int batchSize = (graph.itemNum-n < BATCH_SIZE)? graph.itemNum-n:BATCH_SIZE;
//        Real totalProb = 0;
//        for(Index i = n; i < n + batchSize; i++){
//            std::vector<Real> gredient(WEIGHT_NUM, 0);
//            std::vector<Real> kMax(graph.itemLinkCount[i], 0);
//            std::vector<Index> kMaxIndexArr(graph.itemLinkCount[i]);
//            //std::vector<Real> userProb(graph.userNum);
//            std::cout << "logProb: " << computeLogProb(graph, kMaxIndexArr, i) << std::endl;
//
//            //Real potential;
//            if(graph.itemLinkCount[i] != 0){
//                for(Index u = 0; u < graph.userNum; u++){
//                    recordMax(graph.FPotentialFunc(u, i, 1), u, kMax, kMaxIndexArr);
//                }
//            }
//            int assign;
//            for(Index u = 0; u < graph.userNum; u++){
//                std::vector<Real> f = graph.getFeatures(u, i);
//                Real p = graph.FPotentialFunc(u, i, 1);
//                p = p * p;
//                if(vectorHas(kMaxIndexArr, u)){
//                    assign = 1;
//                }else{
//                    assign = -1;
//                }
//                for(auto k = 0u; k < WEIGHT_NUM; k++){
//                    gredient[k] += f[k] * (assign - (p - 1)/(p + 1));
//                }
//                count++;
//            }
//
//            for(auto k = 0u; k < WEIGHT_NUM; k++){
//                std::cout << std::setw(15) << std::left << graph.weightArr[k] << " " << std::setw(15) << std::left << gredient[k] * learningRate;
//                graph.weightArr[k] += gredient[k] * learningRate;
//                std::cout << " " << std::setw(15) << std::left << graph.weightArr[k] << std::endl;
//            }
//            Real prob = computeLogProb(graph, kMaxIndexArr, i);
//            totalProb += prob;
//
//            std::cout << "after: " << prob << std::endl << std::endl;
//        }
//        std::cout << n << " batch: " << totalProb << std::endl << std::endl;
//        predResult(graph, outFilename);
//    }
//    std::cout << "user: " << graph.userNum << std::endl;
//    std::cout << "item: " << graph.itemNum << std::endl;
//    std::cout << "count: " << count << std::endl;
    return 0;
}
