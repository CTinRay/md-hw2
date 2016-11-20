#include "factor-graph.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>

#ifndef CONSTRUCT_HPP
#define CONSTRUCT_HPP

#define MAX_USER 200000
#define MAX_ITEM 250000
#define MAX_CATEGORY 17000
#define WEIGHT_NUM 6u

typedef std::pair<Index, Index> Pair;
typedef std::pair<Index, int> PairIndexInt;

class ConstructGraph{
    public:
    Index userNum;
    Index itemNum;
    int itemLinkCount[MAX_ITEM];
    Real weightArr[3][WEIGHT_NUM];
    Index taskNum;
    std::vector<Index> taskUser;
    std::vector<Index> taskItem;
    Index maxFriendNum;
    Index maxOwnItemNum;
    Index categoryNum;
    std::vector<std::vector<Index> > userRelation;
    std::vector<std::set<Index> > userItem;
    std::vector<std::set<Index> > itemOwner;
    std::vector<std::set<Index> > itemCategory;
    std::vector<std::vector<Real> > userCategory;
    std::vector<std::set<Index> > category2userSet;
    //std::vector<Real> userPagerank;
    int categoryCount[MAX_CATEGORY];

    // std::vector<std::map<Index, int> > neighbor;
    // std::map<Pair, Index> candidate;  // Pair = (user, item)

    int BFS(Index start, const int maxDistance, Index item);
    // void calculateDistance(int maxDistance);
    // void candidateFilter(int maxDistance);
    public:
    ConstructGraph();
    void insertData(std::string userFile, std::string relationFile, std::string messageFile, std::string predFile);
    //void constructFeatures(const int maxDistance, std::string predFile, std::string outputFile);
    void sampleCandidates(const int maxDistance, const int sampleNum, std::string outputFile);
    // void constructGraph(FactorGraph& graph, const int maxDistance);
    void constructGraph2();
    Real FPotentialFunc(Index user, Index item, int assign);
    std::vector<std::vector<Real>> getFeatures(Index user, Index item);
    Real ItemOwnership(Index user, Index item);
    Real ownerIsFriend(Index user, Index item);
    Real normalizeFriendNum(Index user);
    Real userCategorySimilarity(Index user, Index item);
    Real normalizeNumOwnItem(Index user);
    Real normalizeCategoryPopular(Index item);
};

#endif
