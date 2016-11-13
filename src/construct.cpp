#include "construct.hpp"
#include <fstream>
#include <queue>
#include <iostream>

ConstructGraph::ConstructGraph()
    :userNum(0),
     itemNum(0),
     itemOwner(MAX_ITEM),
     itemCategory(MAX_ITEM),
     neighbor(MAX_USER){};

void ConstructGraph::BFS(Index start, int maxDistance) {
    std::queue<PairIndexInt> q;
    bool visit[userNum];
    for (auto &i: visit){
        i = false;
    }
    
    q.push( PairIndexInt(start, 0) );
    neighbor[start][start] = 0;
    visit[start] = true;
    while (!q.empty()) {
        PairIndexInt k = q.front();
        q.pop();
        if (k.second > maxDistance) {
            break;
        }
        for (auto i: userRelation[k.first]) {
            if (!visit[i]) {
                q.push( PairIndexInt(i, k.second + 1) );
                neighbor[start][i] = k.second + 1;
                visit[i] = true;
            }
        }
    }
}

void ConstructGraph::calculateDistance(int maxDistance) {
    for (auto start = 0u; start < userNum; ++start) {       
        BFS(start, maxDistance);
    }
}

void ConstructGraph::insertData(std::string userFile, std::string relationFile, std::string messageFile){
    /* open file */
    std::fstream userList, relationList, messageList;
    userList.open(userFile, std::fstream::in);
    relationList.open(relationFile, std::fstream::in);
    messageList.open(messageFile, std::fstream::in);
    
    /* handle user.txt */
    Index userIndex;
    while (userList >> userIndex) {
        userNum++;
    }
    userRelation.reserve(userNum);
    userItem.reserve(userNum);
    userCategory.reserve(userNum);
    //neighbor.reserve(userNum);

    /* handle realation.txt */
    Index userIndex1, userIndex2;
    while (relationList >> userIndex1 >> userIndex2) {
        userRelation[userIndex1].push_back(userIndex2);
        userRelation[userIndex2].push_back(userIndex1);
    }
    
    /* handle message.txt */
    std::set<Pair> itemCategorySet;
    for (auto i = 0u; i < MAX_CATEGORY; ++i) {
        categoryCount[i] = 0;
    }
    int linkCount;
    Index itemIndex, categoryIndex;
    while (messageList >> userIndex >> itemIndex >> categoryIndex >> linkCount) {
        itemNum = ( itemNum < itemIndex ) ? itemIndex : itemNum;

        if ( userItem[userIndex].find(itemIndex) != userItem[userIndex].end() ) {
            userItem[userIndex].insert(itemIndex);
        }
        
        if ( itemOwner[itemIndex].find(userIndex) != itemOwner[itemIndex].end() ) {
            itemOwner[itemIndex].insert(userIndex);
        }
         
        if ( itemCategory[itemIndex].find(categoryIndex) != itemCategory[itemIndex].end() ) {
            itemCategory[itemIndex].insert(categoryIndex);
        }
        
        bool has_category = false;
        for (auto &it: userCategory[userIndex]) {
            if (it.first == categoryIndex) {
                has_category = true;
                it.second += 1;
            }
        }
        if (!has_category) {
            userCategory[userIndex].push_back( Pair(categoryIndex, 1) );
        }
        
        if ( itemCategorySet.find( Pair(itemIndex, categoryIndex) ) != itemCategorySet.end() ) {
            categoryCount[categoryIndex]++; 
            itemCategorySet.insert( Pair(itemIndex, categoryIndex) );
        }

        itemLinkCount[itemIndex] = linkCount;
    }
}

void ConstructGraph::candidateFilter(int maxDistance){
    calculateDistance(maxDistance);
    Index scope = 0;
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
}

void ConstructGraph::constructGraph(FactorGraph& graph, int maxDistance){
    candidateFilter(maxDistance); 
    for (auto c: candidate) {
        Real shortest = INFINITY;
        for (auto k: itemOwner[c.first.second]) {
            if (neighbor[c.first.first].find(k) != neighbor[c.first.first].end()) {
                if (neighbor[c.first.first][k] < shortest) {
                    shortest = neighbor[c.first.first][k];
                }
            }
        }
        Real UF = 1.0 / shortest;
        Real IO = ( shortest == 0 ) ? 1 : 0;
        Real CP = 0;
        for (auto i: itemCategory[c.first.second]) {
           CP += categoryCount[i]; 
        }
        std::vector<Index> scope = {c.second};
        std::vector<Real> features = {UF, IO, CP};
        FFactorFunction fFactorFunction(features);
        graph.addFactor( scope, fFactorFunction );
    }
}
