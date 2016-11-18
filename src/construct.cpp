#include "construct.hpp"
#include <fstream>
#include <queue>
#include <random>
#include <iostream>
#include <sstream>
#include <cmath>
#include <omp.h>

#define GREATER_THAN_MAX_DISTANCE -1
#define MAX_PREDICTION 600000

ConstructGraph::ConstructGraph()
    :userNum(0),
     itemNum(0),
     categoryNum(0),
     itemOwner(MAX_ITEM),
     itemCategory(MAX_ITEM){};

int ConstructGraph::BFS(Index start, const int maxDistance, Index item, const int direction=DIRECTED) {
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
    visit[start] = true;
    while (!q.empty()) {
        PairIndexInt k = q.front();
        q.pop();
        if (k.second + 1 > maxDistance) {
            break;
        }
        if (direction == DIRECTED || direction == UNDIRECTED) {
            for (auto i: userRelation[k.first]) {
                if (!visit[i]) {
                    for (auto owner: itemOwner[item]) {
                        if (i == owner) {
                            return k.second + 1;
                        }
                    }
                    q.push( PairIndexInt(i, k.second + 1) );
                    visit[i] = true;
                }
            }
        }
        if (direction == REVERSE || direction == UNDIRECTED) {
            for (auto i: userRelationReverse[k.first]) {
                if (!visit[i]) {
                    for (auto owner: itemOwner[item]) {
                        if (i == owner) {
                            return k.second + 1;
                        }
                    }
                    q.push( PairIndexInt(i, k.second + 1) );
                    visit[i] = true;
                }
            }
        }
    }
    return GREATER_THAN_MAX_DISTANCE;
}

void userCategoryNormalization(std::vector<std::vector<double> >& userCategory, std::vector<std::set<Index> >& userItem,
                               int* categoryCount, const Index categoryNum, const Index userNum) {
    for (auto user = 0u; user < userNum; ++user) {
        if (userItem[user].size() > 0) {
            int total = 0;
            for (auto cat = 0u; cat < categoryNum; ++cat) {
                total += userCategory[user][cat];
            }
            for (auto cat = 0u; cat < categoryNum; ++cat) {
                userCategory[user][cat] = userCategory[user][cat] / total;
            }
        }
    }
}

void ConstructGraph::insertData(std::string userFile, std::string relationFile,
                                std::string messageFile ,std::string pagerankFile) {
    /* handle user.txt */
    std::ifstream userList(userFile);
    Index userIndex;
    while (userList >> userIndex) {
        userNum++;
    }
    userList.close();
    
    /* Assign size */
    userRelation.resize(userNum);
    userRelationReverse.resize(userNum);
    userItem.resize(userNum);
    userCategory.resize(userNum);
    for (auto i = 0u; i < userNum; ++i) {
        userCategory[i].resize(MAX_CATEGORY);
        for (auto j = 0u; j < MAX_CATEGORY; ++j) {
            userCategory[i][j] = 0;
        }
    }
    userPagerank.resize(userNum);

    /* handle realation.txt */
    std::ifstream relationList(relationFile);
    Index userIndex1, userIndex2;
    while (relationList >> userIndex1 >> userIndex2) {
        userRelation[userIndex1].push_back(userIndex2);
        userRelationReverse[userIndex2].push_back(userIndex1);
    }
    relationList.close();

    /* handle message.txt */
    std::ifstream messageList(messageFile);
    std::set<Pair> itemCategorySet;
    int linkCount;
    Index itemIndex, categoryIndex;
    while (messageList >> userIndex >> itemIndex >> categoryIndex >> linkCount) {
        itemNum = ( itemNum < itemIndex + 1 ) ? itemIndex + 1 : itemNum;
        categoryNum = ( categoryNum < categoryIndex + 1 ) ? categoryIndex + 1 : categoryNum;
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
    messageList.close();

    for (auto i = 0u; i < userNum; ++i) {
        userCategory[i].resize(categoryNum);
        userCategory[i].shrink_to_fit();
    }
    
    /* handle pagerank */
    std::ifstream pagerank(pagerankFile);
    std::string equalsign;
    while (pagerank >> userIndex) {
        pagerank >> equalsign >> userPagerank[userIndex];
    }
    pagerank.close();

    userCategoryNormalization(userCategory, userItem, categoryCount, categoryNum, userNum);
    std::cout << "insertData finished" << std::endl;
}

// Only consider candidates in pred.id since there are too many possible candidates.
// Features: UserFriendship, ItemOwnership, (perFriendLinkCount), userFriendNum,
//           # of items owned by the user, (itemOwnerPagerank), caterogyInnerProduct
void ConstructGraph::constructFeatures(const int maxDistance, const int direction, std::string predFile, std::string outputFile) {
    /* Read and store links for prediction */
    std::ifstream pred(predFile); 
    Index* user = new Index[MAX_PREDICTION];
    Index* item = new Index[MAX_PREDICTION];
    std::string questionmark;
    int task = 0;
    while (pred >> user[task] >> item[task] >> questionmark) {
        task++;
    }
    pred.close();

    /* calculate usersDistance */
    int* usersDistance = new int[MAX_PREDICTION];
    int pass = 0;
    for (auto i = 0; i < task; ++i) {
        usersDistance[i] = BFS(user[i], maxDistance, item[i], direction);
        pass = (usersDistance[i] != GREATER_THAN_MAX_DISTANCE) ? pass + 1 : pass;
    }
    std::cout << "Ratio within distance " << maxDistance << ": " << (double)pass / task << std::endl;
    
    /* calculate catergoryInnerProduct */
    Real* categoryInnerProduct = new Real[MAX_PREDICTION];
    int positiveCategoryInnerProduct = 0;
    for (auto i = 0; i < task; ++i) {
        categoryInnerProduct[i] = 0; 
        for (auto cat = 0u; cat < categoryNum; ++cat) {
            for (auto owner: itemOwner[item[i]]) {
                categoryInnerProduct[i] += userCategory[user[i]][cat] * userCategory[owner][cat];
            }
        }
        if (categoryInnerProduct[i] > 0) {
            positiveCategoryInnerProduct++;
        }
    }
    std::cout << "Positive categoryInnerProduct ratio: " << (double)positiveCategoryInnerProduct / task << std::endl;

    /* output features */
    std::ofstream output(outputFile);
    double totalUFN = 0;
    double totalOFN = 0;
    double totalPFLC = 0;
    double totalOwnedNum = 0;
    double totalCIP = 0;
    for (auto i = 0; i < task; ++i) {
        Real UF = (usersDistance[i] == GREATER_THAN_MAX_DISTANCE || usersDistance[i] == 0) ? 0 : 1.0 / usersDistance[i];
        Real IO = (usersDistance[i] == 0) ? 1 : 0;
        Real itemOwnerFriendNum = 1; // psuedocount
        for (auto owner: itemOwner[item[i]]) {
            itemOwnerFriendNum += userRelation[owner].size();
        }
        
        Real itemOwnerPagerank = 0;
        for (auto owner: itemOwner[item[i]]) {
            itemOwnerPagerank += ( userPagerank[owner] * userPagerank[owner] );
        }
        itemOwnerPagerank = sqrt(itemOwnerPagerank);

        totalUFN += userRelation[user[i]].size();
        totalOFN += itemOwnerFriendNum;
        totalPFLC += itemLinkCount[item[i]] / itemOwnerFriendNum;
        totalOwnedNum += userItem[user[i]].size();
        totalCIP += categoryInnerProduct[i];

        output << UF << ' '
               << IO << ' ' 
               // << itemLinkCount[item[i]] / itemOwnerFriendNum << ' '
               << userRelation[user[i]].size() << ' '
               // << itemOwnerFriendNum << ' '
               << userItem[user[i]].size() << ' '
               // << itemOwnerPagerank << ' '
               << categoryInnerProduct[i] << std::endl;
    }
    std::cout << "average userFriendNum: " << totalUFN / task << std::endl;
    // std::cout << "average ownerFriendNum: " << totalOFN / task << std::endl;
    // std::cout << "average perFriendLinkCount: " << totalPFLC / task << std::endl;
    std::cout << "average OwnedNum: " << totalOwnedNum / task << std::endl;
    std::cout << "average CIP: " << totalCIP / task << std::endl;
    output.close();

    delete[] user;
    delete[] item;
    delete[] usersDistance;
    delete[] categoryInnerProduct;
}

// rd and gen have been defined in gibbs_sampler
std::random_device rd2;
std::mt19937_64 gen2(rd2());

Index randomIndex(const int maxIndex) {
    std::uniform_int_distribution<Index> dis(0, maxIndex - 1);
    return dis(gen2);
}

void ConstructGraph::sampleCandidates(const int sampleNum, std::string outputFile) {
    std::ofstream output(outputFile);
    for (auto i = 0; i < sampleNum; ++i) {
        Index user = randomIndex(userNum);
        Index item = randomIndex(itemNum);
        while (itemLinkCount[item] == 0) {
            item = randomIndex(itemNum);
        }
        output << user << ' ' << item << ' ' << '?' << std::endl;
    }
    output.close();
    std::cout << "sample finished" << std::endl;
}

void insertCandidate(std::string predFile, const int userNum,
                     std::map<Pair, Index>& candidate, std::vector<std::vector<Pair> >& candidateUserInfo) {
    std::ifstream pred(predFile);
    Index user, item, scope = 0;
    std::string questionmark;
    candidateUserInfo.resize(userNum);
    while (pred >> user >> item >> questionmark) {
        candidateUserInfo[user].push_back( Pair(item, scope) );
        candidate[Pair(user, item)] = scope++;
    }
    pred.close();
}

void ConstructGraph::addFFactorFunction(FactorGraph& graph, std::string featuresFile) {
    std::ifstream features(featuresFile);
    std::string featuresLine;
    for (auto c: candidate) {
        std::vector<Index> scopes = {c.second};
        std::getline(features, featuresLine);
        std::istringstream iss(featuresLine);
        std::vector<Real> featuresVector;
        Real f;
        while (iss >> f) {
            featuresVector.push_back(f);
        }
        FFactorFunction *fFactorFunction = new FFactorFunction(featuresVector);
        graph.addFactor( scopes, *fFactorFunction );
    }
    features.close();
    std::cout << "constructGraph layer1 finished" << std::endl;
}

void ConstructGraph::addGFactorFunction(FactorGraph& graph, 
                                        GFactorFunction* OIFunc, GFactorFunction* FIFunc, GFactorFunction* CCFunc) {
    for (auto u = 0u; u < userNum; ++u) {
        for (auto info1: candidateUserInfo[u]) {
            for (auto info2: candidateUserInfo[u]) {
                Index i1 = info1.first;
                Index i2 = info2.first;
                if (i2 <= i1) {
                    continue;
                }
                std::vector<Index>* theSameOwner = getTheSameOwner(i1, i2);
                if (theSameOwner->size() == 0) {
                    delete theSameOwner;
                    continue;
                }
                bool friendship = false;
                for (auto owner: *theSameOwner) {
                    if (theyAreFriends(u, owner)) {
                        friendship = true;
                        break;
                    }
                }
                std::vector<Index> scopes = {info1.second, info2.second};
                if (friendship) {
                    graph.addFactor(scopes, *FIFunc);
                }
                else {
                    graph.addFactor(scopes, *OIFunc);
                }
                delete theSameOwner;
            }
        }
    }
    for (auto u = 0u; u < userNum; ++u) {
        for (auto info1: candidateUserInfo[u]) {
            for (auto info2: candidateUserInfo[u]) {
                Index i1 = info1.first;
                Index i2 = info2.first;
                if (i2 <= i1 || !haveTheSameCategory(i1, i2)) {
                    continue;
                }
                std::vector<Index> scopes = {info1.second, info2.second};
                graph.addFactor(scopes, *CCFunc);
            }
        }
    }
    std::cout << "constructGraph layer2 finished" << std::endl;
}

void ConstructGraph::constructGraph(FactorGraph& graph,
                                    GFactorFunction* OIFunc, GFactorFunction* FIFunc, GFactorFunction* CCFunc, 
                                    std::string predFile, std::string featuresFile) {
    insertCandidate(predFile, userNum, candidate, candidateUserInfo);
    graph.nVars = candidate.size();
    addFFactorFunction(graph, featuresFile);
    addGFactorFunction(graph, OIFunc, FIFunc, CCFunc);
}

bool ConstructGraph::theyAreFriends(Index i, Index j){
    for(auto f: userRelation[i]){
        if(f == j){
            return true;
        }
    }
    return false;
}

std::vector<Index>* ConstructGraph::getTheSameOwner(Index i1, Index i2) {
    std::vector<Index>* theSameOwner = new std::vector<Index>;
    for (auto owner1: itemOwner[i1]) {
        if (userItem[owner1].find(i2) != userItem[owner1].end()) {
            theSameOwner->push_back(owner1);
        }
    }
    return theSameOwner;
}

bool ConstructGraph::haveTheSameCategory(Index i1, Index i2) {
    for (auto cat1: itemCategory[i1]) {
        if (itemCategory[i2].find(cat1) != itemCategory[i2].end()) {
            return true;
        }
    }
    return false;
}
