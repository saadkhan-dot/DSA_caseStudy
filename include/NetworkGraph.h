#ifndef NETWORK_GRAPH_H
#define NETWORK_GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set> /*if used array insted youd have to create aloop to locate the place where u visited*/
#include <utility>
#include <stack>

struct LinkAction {
    std::string nodeA;
    std::string nodeB;
};

// ===================== EDGE STRUCT =====================
struct Edge {
    std::string to;
    int weight;       // latency / cost in ms
    bool active;      // false = severed link (DFS reroute trigger) if i didnt use this then the link would be deleted and then would have to restore it manually
};

// ===================== NETWORK GRAPH =====================
// Represents the intelligence communication network.
// Nodes  = station/relay names (e.g. "STATION_ZULU_9")
// Edges  = direct communication links with weight (latency)
class NetworkGraph {
public:
    NetworkGraph();

    void addNode(const std::string& name);
    bool addEdge(const std::string& a, const std::string& b, int weight);
    bool rollbackLastAction();

    // Marks a link as severed (both directions). Returns false if edge not found.
    bool severLink(const std::string& a, const std::string& b);

    // Restores a previously severed link.
    bool restoreLink(const std::string& a, const std::string& b);

    bool hasNode(const std::string& name) const;

    // Dijkstra shortest path over ACTIVE edges only.
    // Returns the path as a vector of node names (empty if no path found).
    std::vector<std::string> findShortestPath(const std::string& src,
                                                const std::string& dest,
                                                int& totalCost) const;

    // DFS-based failure recovery: explores alternative routes when the
    // primary link is down, returns all reachable nodes from src using
    // only ACTIVE edges (i.e. backup channels still usable), and the
    // actual DFS traversal order.
    std::vector<std::string> dfsBackupChannels(const std::string& src,
                                                 const std::string& dest,
                                                 std::vector<std::string>& traversalOrder) const;

    std::vector<std::string> getAllNodes() const;
    std::vector<Edge> getEdges(const std::string& node) const;

    // Renders a simple ASCII adjacency table showing link status
    std::string renderASCIITable() const;

private:
    std::unordered_map<std::string, std::vector<Edge>> adjList;
    std::stack<LinkAction> rollbackStack;

    void dfsHelper(const std::string& current,
                    const std::string& dest,
                    std::set<std::string>& visited,
                    std::vector<std::string>& path,
                    std::vector<std::string>& traversalOrder,
                    bool& found) const;
};

#endif // NETWORK_GRAPH_H
