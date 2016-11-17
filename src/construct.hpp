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

#ifndef MAX_CATEGORY
#define MAX_CATEGORY 17000
#endif

#define DIRECTED 1
#define REVERSE 2
#define UNDIRECTED 3

typedef std::pair<Index, Index> Pair;
typedef std::pair<Index, int> PairIndexInt;

class ConstructGraph{
private:
    Index userNum;
    Index itemNum;
    Index categoryNum;
    std::vector<std::vector<Index> > userRelation;
    std::vector<std::vector<Index> > userRelationReverse;
    std::vector<std::set<Index> > userItem;
    std::vector<std::set<Index> > itemOwner;
    std::vector<std::set<Index> > itemCategory;
    std::vector<std::vector<double> > userCategory; // distribution
    std::vector<Real> userPagerank;
    int categoryCount[MAX_CATEGORY];
    int itemLinkCount[MAX_ITEM];
    std::map<Pair, Index> candidate;  // <(user, item), scope>

    int BFS(Index start, const int maxDistance, Index item, const int direction);
    bool theyAreFriends(Index i, Index j);
public:
    ConstructGraph();
    void insertData(std::string userFile, std::string relationFile, std::string messageFile, std::string pagerankFile);
    void constructFeatures(const int maxDistance, const int direction, // DIRECTED, REVERSE, UNDIRECTED
                            std::string predFile, std::string outputFile);
    void sampleCandidates(const int sampleNum, std::string outputFile);
    void constructGraph(FactorGraph& graph, std::string predFile, std::string featuresFile); // must run insertData first
};

#endif
