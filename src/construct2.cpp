#include "construct2.hpp"
#include <fstream>
#include <queue>
#include <random>
#include <iostream>
#include <cmath>
#include <omp.h>

#define GREATER_THAN_MAX_DISTANCE -1
#define MAX_PREDICTION 800000
#define EXP expl

ConstructGraph::ConstructGraph()
    :userNum(0),
     itemNum(0),
     taskNum(0),
     categoryNum(0),
     itemOwner(MAX_ITEM),
     itemCategory(MAX_ITEM){
         for(auto i = 0u; i < WEIGHT_NUM; i++){
             weightArr[i] = 0.1;
         }
         weightArr[WEIGHT_NUM-2] = 0.01;
     };

int ConstructGraph::BFS(Index start, const int maxDistance, Index item) {
    std::queue<PairIndexInt> q;
    bool visit[userNum];
    for (auto &i: visit){
        i = false;
    }
    
    for (auto owner: itemOwner[item]) {
        if (start == owner) {
            return 0;
        }
    }
    q.push( PairIndexInt(start, 0) );
    // neighbor[start][start] = 0;
    visit[start] = true;
    while (!q.empty()) {
        PairIndexInt k = q.front();
        q.pop();
        if (k.second + 1 > maxDistance) {
            break;
        }
        for (auto i: userRelation[k.first]) {
            if (!visit[i]) {
                for (auto owner: itemOwner[item]) {
                    if (i == owner) {
                        return k.second + 1;
                    }
                }
                q.push( PairIndexInt(i, k.second + 1) );
                // neighbor[start][i] = k.second + 1;
                visit[i] = true;
            }
        }
    }
    return GREATER_THAN_MAX_DISTANCE;
}

/*void ConstructGraph::calculateDistance(int maxDistance) {
    for (auto start = 0u; start < userNum; ++start) {       
        BFS(start, maxDistance);
    }
}*/

void ConstructGraph::insertData(std::string userFile, std::string relationFile,
                                std::string messageFile ,std::string predFile) {
    std::ifstream pred(predFile); 
    std::string questionmark;
    Index userIndex;
    Index itemIndex, categoryIndex;
    while (pred >> userIndex >> itemIndex >> questionmark) {
        taskUser.push_back(userIndex);
        taskItem.push_back(itemIndex);
        taskNum++;
    }
    pred.close();
    /* handle user.txt */
    std::ifstream userList(userFile);
    while (userList >> userIndex) {
        userNum++;
    }
    userList.close();
    
    /* Assign size */
    userRelation.resize(userNum);
    userItem.resize(userNum);
    userCategory.resize(userNum);
    for (auto i = 0u; i < userNum; ++i) {
        userCategory[i].resize(MAX_CATEGORY);
        for (auto j = 0u; j < MAX_CATEGORY; ++j) {
            userCategory[i][j] = 0;
        }
    }
    //userPagerank.resize(userNum);
    // neighbor.resize(userNum);

    /* handle realation.txt */
    std::ifstream relationList(relationFile);
    Index userIndex1, userIndex2;
    while (relationList >> userIndex1 >> userIndex2) {
        userRelation[userIndex1].push_back(userIndex2);
        //userRelation[userIndex2].push_back(userIndex1);
    }
    relationList.close();

    /* handle message.txt */
    std::ifstream messageList(messageFile);
    std::set<Pair> itemCategorySet;
    for (auto i = 0u; i < MAX_CATEGORY; ++i) {
        categoryCount[i] = 0;
    }
    int linkCount;
    while (messageList >> userIndex >> itemIndex >> categoryIndex >> linkCount) {
        itemNum = ( itemNum < itemIndex ) ? itemIndex : itemNum;
        categoryNum = ( categoryNum < categoryIndex ) ? categoryIndex : categoryNum;
        userCategory[userIndex][categoryIndex]++; 
        itemLinkCount[itemIndex] = linkCount;

        if ( userItem[userIndex].find(itemIndex) == userItem[userIndex].end() ) {
            userItem[userIndex].insert(itemIndex);
        }
        
        if ( itemOwner[itemIndex].find(userIndex) == itemOwner[itemIndex].end() ) {
            itemOwner[itemIndex].insert(userIndex);
        }
         
        if ( itemCategory[itemIndex].find(categoryIndex) == itemCategory[itemIndex].end() ) {
            itemCategory[itemIndex].insert(categoryIndex);
        }
        
        if ( itemCategorySet.find( Pair(itemIndex, categoryIndex) ) == itemCategorySet.end() ) {
            categoryCount[categoryIndex]++; 
            itemCategorySet.insert( Pair(itemIndex, categoryIndex) );
        }
    }
    itemNum++;
    categoryNum++;
    messageList.close();

    /* handle pagerank */
    //std::ifstream pagerank(pagerankFile);
    //std::string equalsign;
    //while (pagerank >> userIndex) {
    //    pagerank >> equalsign >> userPagerank[userIndex];
    //}
    //pagerank.close();
    std::cout << "insertData finished" << std::endl;
}

void userCategoryNormalization(std::vector<std::vector<double> >& userCategory,
                               int* categoryCount, const Index categoryNum, const Index userNum) {
    for (auto cat = 0u; cat < categoryNum; ++cat) {
        double average = (Real) categoryCount[cat] / userNum;
        double standardDeviation = 0;
        for (auto user = 0u; user < userNum; ++user) {
            standardDeviation += ( userCategory[user][cat] - average ) * ( userCategory[user][cat] - average );
        }
        standardDeviation = sqrt(standardDeviation); 
        for (auto user = 0u; user < userNum; ++user) {
            userCategory[user][cat] = ( userCategory[user][cat] - average ) / standardDeviation;
        }
    }
}

// Only consider candidates in pred.id since there are too many possible candidates.
// Remove "neightbor" which used to store results of BFS due to memory consideration.
//
// Features: UserFriendship, ItemOwnership, CategoryPopularity, 
//           itemLinkCount, # of items owned by the user, itemOwnerPagerank, caterogyInnerProduct
//void ConstructGraph::constructFeatures(const int maxDistance, std::string predFile, std::string outputFile) {
//    /* Read and store links for prediction */
//    std::ifstream pred(predFile); 
//    Index* user = new Index[MAX_PREDICTION];
//    Index* item = new Index[MAX_PREDICTION];
//    std::string questionmark;
//    int task = 0;
//    while( pred >> user[task] >> item[task] >> questionmark ) {
//        task++;
//    }
//    pred.close();
//
//    /* calculate usersDistance */
//    int* usersDistance = new int[MAX_PREDICTION];
//    int pass = 0;
//    #pragma omp parallel for num_threads(10)
//    for (auto i = 0; i < task; ++i) {
//        usersDistance[i] = BFS(user[i], maxDistance, item[i]);
//        pass = (usersDistance[i] != GREATER_THAN_MAX_DISTANCE) ? pass + 1 : pass;
//    }
//    std::cout << "Ratio within distance " << maxDistance << ": " << (double)pass / task << std::endl;
//    
//    /* calculate catergoryInnerProduct */
//    userCategoryNormalization(userCategory, categoryCount, categoryNum, userNum);
//    std::cout << "normalization finished" << std::endl;
//    Real* categoryInnerProduct = new Real[MAX_PREDICTION];
//    #pragma omp parallel for num_threads(10)
//    for (auto i = 0; i < task; ++i) {
//        categoryInnerProduct[i] = 0;
//        for (auto cat = 0u; cat < categoryNum; ++cat) {
//            for (auto owner: itemOwner[item[i]]) {
//                categoryInnerProduct[i] += userCategory[user[i]][cat] * userCategory[owner][cat];
//            }
//        }
//    }
//
//    /* output features */
//    std::ofstream output(outputFile);
//    for (auto i = 0; i < task; ++i) {
//        Real UF = (usersDistance[i] == GREATER_THAN_MAX_DISTANCE || usersDistance[i] == 0) ? 0 : 1.0 / usersDistance[i];
//        Real IO = (usersDistance[i] == 0) ? 1 : 0;
//        Real CP = 0;
//        for (auto cat: itemCategory[item[i]]) {
//           CP += categoryCount[cat]; 
//        }
//        Real itemOwnerPagerank = 0;
//        for (auto owner: itemOwner[item[i]]) {
//            itemOwnerPagerank += ( userPagerank[owner] * userPagerank[owner] );
//        }
//        itemOwnerPagerank = sqrt(itemOwnerPagerank);
//
//        output << UF << ' ' 
//               << IO << ' ' 
//               << CP << ' ' 
//               << itemLinkCount[item[i]] << ' '
//               << userItem[user[i]].size() << ' '
//               << itemOwnerPagerank << ' '
//               << categoryInnerProduct[i] << std::endl;
//    }
//    output.close();
//}

// rd and gen have been defined in gibbs_sampler
std::random_device rd2;
std::mt19937_64 gen2(rd2());

Index randomIndex(const int maxIndex) {
    std::uniform_int_distribution<Index> dis(0, maxIndex - 1);
    return dis(gen2);
}

void ConstructGraph::sampleCandidates(const int maxDistance, const int sampleNum, std::string outputFile) {
    std::ofstream output(outputFile);
    #pragma omp parallel for num_threads(10)
    for (auto i = 0; i < sampleNum; ++i) {
        Index user = randomIndex(userNum);
        while (userItem[user].size() == 0) {
            user = randomIndex(userNum);
        }
        Index item = randomIndex(itemNum);
        while (itemLinkCount[item] == 0) {
            item = randomIndex(itemNum);
        }
        output << user << ' ' << item << ' ' << '?' << std::endl;
    }
    std::cout << "sample finished" << std::endl;
    output.close();
}

/*void ConstructGraph::candidateFilter(const int maxDistance){
    calculateDistance(maxDistance);
    std::cout << "calculateDistance finished" << std::endl;
    Index scope = 0;
    #pragma omp parallel for num_threads(10)
    for (auto i = 0u; i < userNum; ++i) {
        for (auto j = 0u; j < itemNum; ++j) {
            for(auto k: itemOwner[j]) {
                if (neighbor[i].find(k) != neighbor[i].end()) {
                    candidate[ Pair(i, j) ] = scope++;
                    break;
                }
            }
            // TODO: inner product
        }
    }
    std::cout << "candidateNum: " << scope << std::endl; 
}*/

void ConstructGraph::constructGraph2(){
    
}
// Construct factor graph (remove in this version)
/*void ConstructGraph::constructGraph(FactorGraph& graph, int maxDistance){
    candidateFilter(maxDistance); 
    std::cout << "candidateFilter finished" << std::endl;
    for (auto c: candidate) {
        Real shortest = INFINITY;
        for (auto k: itemOwner[c.first.second]) {
            if (neighbor[c.first.first].find(k) != neighbor[c.first.first].end()) {
                if (neighbor[c.first.first][k] < shortest) {
                    shortest = neighbor[c.first.first][k];
                }
            }
        }
        Real UF = ( shortest == 0 ) ? 1 : 1.0 / shortest;
        Real IO = ( shortest == 0 ) ? 1 : 0;
        Real CP = 0;
        for (auto i: itemCategory[c.first.second]) {
           CP += categoryCount[i]; 
        }
        std::vector<Index> scope = {c.second};
        std::vector<Real> features = {UF, IO, CP};
        FFactorFunction *fFactorFunction = new FFactorFunction(features);
        graph.addFactor( scope, *fFactorFunction );
    }
}*/
Real ConstructGraph::FPotentialFunc(Index user, Index item, int assign){
    std::vector<Real> subPotentialArr = getFeatures(user, item);
    Real sum = 0;
    for(auto i = 0u; i < WEIGHT_NUM; i++){
        //std::cout << subPotentialArr[i] << std::endl;
        sum += weightArr[i] * subPotentialArr[i] * assign;
    }
    return EXP(sum);
}
std::vector<Real> ConstructGraph::getFeatures(Index user, Index item){
    std::vector<Real> subPotentialArr = {ItemOwnership(user, item), ownerIsFriend(user, item), friendNum(user),
                                         userCategorySimilarity(user, item), categoryPopular(item), (Real)itemLinkCount[item]};
    return subPotentialArr;
}

Real ConstructGraph::ItemOwnership(Index user, Index item){
    if(itemOwner[item].find(user) != itemOwner[item].end()){
        return 1.0;
    }else{
        return 0.0;
    }
}
Real ConstructGraph::ownerIsFriend(Index user, Index item){
    for(auto f: userRelation[user]){
        if(itemOwner[item].find(f) != itemOwner[item].end()){
            return 1.0;
        }
    }
    return 0.0;
}
Real ConstructGraph::friendNum(Index user){
    return userRelation[user].size();
}
Real ConstructGraph::userCategorySimilarity(Index user, Index item){
    double max = 0.0;
    for(auto c: itemCategory[item]){
        max = (userCategory[user][c] > max)? userCategory[user][c]:max;
    }
    return max;
}
Real ConstructGraph::categoryPopular(Index item){
    Real sum = 0;
    for(auto c: itemCategory[item]){
        sum += categoryCount[c];
    }
    return sum / categoryNum;
}
