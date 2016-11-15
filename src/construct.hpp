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
#define MAX_CATEGORY 20000

typedef std::pair<Index, Index> Pair;
typedef std::pair<Index, int> PairIndexInt;

class ConstructGraph{
private:
    Index userNum;
    Index itemNum;
    Index categoryNum;
    std::vector<std::vector<Index> > userRelation;
    std::vector<std::set<Index> > userItem;
    std::vector<std::set<Index> > itemOwner;
    std::vector<std::set<Index> > itemCategory;
    std::vector<std::vector<double> > userCategory;
    std::vector<Real> userPagerank;
    int categoryCount[MAX_CATEGORY];
    int itemLinkCount[MAX_ITEM];

    // std::vector<std::map<Index, int> > neighbor;
    std::map<Pair, Index> candidate;  // Pair = (user, item)

    int BFS(Index start, const int maxDistance, Index item);
    // void calculateDistance(int maxDistance);
    // void candidateFilter(int maxDistance);
public:
    ConstructGraph();
    void insertData(std::string userFile, std::string relationFile, std::string messageFile, std::string pagerankFile);
    void constructFeatures(const int maxDistance, std::string predFile, std::string outputFile);
    // void constructGraph(FactorGraph& graph, const int maxDistance);
};

#endif
