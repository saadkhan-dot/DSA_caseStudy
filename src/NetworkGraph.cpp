#include "NetworkGraph.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <sstream>
#include <iomanip>

NetworkGraph::NetworkGraph() {}

void NetworkGraph::addNode(const std::string& name) {
    if (adjList.find(name) == adjList.end()) {
        adjList[name] = {};
    }
}

bool NetworkGraph::addEdge(const std::string& a, const std::string& b, int weight) {
    if (a.empty() || b.empty() || weight < 0) return false;
    addNode(a);
    addNode(b);
    adjList[a].push_back({b, weight, true});
    adjList[b].push_back({a, weight, true});
    return true;
}

bool NetworkGraph::severLink(const std::string& a, const std::string& b) {
    bool found = false;
    if (adjList.count(a)) {
        for (auto& e : adjList[a]) {
            if (e.to == b) { e.active = false; found = true; }
        }
    }
    if (adjList.count(b)) {
        for (auto& e : adjList[b]) {
            if (e.to == a) { e.active = false; found = true; }
        }
    }
    if (found) {
    rollbackStack.push({a, b});
    }
    return found;
}

bool NetworkGraph::restoreLink(const std::string& a, const std::string& b) {
    bool found = false;
    if (adjList.count(a)) {
        for (auto& e : adjList[a]) {
            if (e.to == b) { e.active = true; found = true; }
        }
    }
    if (adjList.count(b)) {
        for (auto& e : adjList[b]) {
            if (e.to == a) { e.active = true; found = true; }
        }
    }
    return found;
}

bool NetworkGraph::hasNode(const std::string& name) const {
    return adjList.find(name) != adjList.end();
}

bool NetworkGraph::rollbackLastAction() {

    if (rollbackStack.empty())
        return false;

    LinkAction last = rollbackStack.top();
    rollbackStack.pop();

    return restoreLink(last.nodeA, last.nodeB);
}

std::vector<std::string> NetworkGraph::findShortestPath(const std::string& src,
                                                          const std::string& dest,
                                                          int& totalCost) const {
    totalCost = -1;
    if (!hasNode(src) || !hasNode(dest)) return {};

    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> prev;
    for (const auto& kv : adjList) dist[kv.first] = std::numeric_limits<int>::max();
    dist[src] = 0;

    using PQItem = std::pair<int, std::string>; // (distance, node)
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == dest) break;

        for (const auto& edge : adjList.at(u)) {
            if (!edge.active) continue; // skip severed links
            int nd = d + edge.weight;
            if (nd < dist[edge.to]) {
                dist[edge.to] = nd;
                prev[edge.to] = u;
                pq.push({nd, edge.to});
            }
        }
    }

    if (dist[dest] == std::numeric_limits<int>::max()) return {}; // unreachable

    // Reconstruct path
    std::vector<std::string> path;
    std::string cur = dest;
    path.push_back(cur);
    while (cur != src) {
        if (prev.find(cur) == prev.end()) return {}; // safety
        cur = prev[cur];
        path.push_back(cur);
    }
    std::reverse(path.begin(), path.end());
    totalCost = dist[dest];
    return path;
}

void NetworkGraph::dfsHelper(const std::string& current,
                              const std::string& dest,
                              std::set<std::string>& visited,
                              std::vector<std::string>& path,
                              std::vector<std::string>& traversalOrder,
                              bool& found) const {
    if (found) return;
    visited.insert(current);
    traversalOrder.push_back(current);
    path.push_back(current);

    if (current == dest) {
        found = true;
        return;
    }

    if (adjList.count(current)) {
        for (const auto& edge : adjList.at(current)) {
            if (!edge.active) continue; // can't traverse severed links
            if (visited.count(edge.to)) continue;
            dfsHelper(edge.to, dest, visited, path, traversalOrder, found);
            if (found) return;
        }
    }
    path.pop_back(); // backtrack
}

std::vector<std::string> NetworkGraph::dfsBackupChannels(const std::string& src,
                                                           const std::string& dest,
                                                           std::vector<std::string>& traversalOrder) const {
    traversalOrder.clear();
    std::vector<std::string> path;
    if (!hasNode(src) || !hasNode(dest)) return path;

    std::set<std::string> visited;
    bool found = false;
    dfsHelper(src, dest, visited, path, traversalOrder, found);

    if (!found) return {}; // no backup channel exists
    return path;
}

std::vector<std::string> NetworkGraph::getAllNodes() const {
    std::vector<std::string> nodes;
    for (const auto& kv : adjList) nodes.push_back(kv.first);
    std::sort(nodes.begin(), nodes.end());
    return nodes;
}

std::vector<Edge> NetworkGraph::getEdges(const std::string& node) const {
    if (adjList.count(node)) return adjList.at(node);
    return {};
}

std::string NetworkGraph::renderASCIITable() const {
    std::ostringstream oss;
    oss << "+----------------------+----------------------+--------+----------+\n";
    oss << "| NODE                 | LINKED TO            | WEIGHT | STATUS   |\n";
    oss << "+----------------------+----------------------+--------+----------+\n";

    auto nodes = getAllNodes();
    for (const auto& n : nodes) {
        for (const auto& e : adjList.at(n)) {
            oss << "| " << std::left << std::setw(20) << n
                << " | " << std::left << std::setw(20) << e.to
                << " | " << std::right << std::setw(6) << e.weight
                << " | " << std::left << std::setw(8) << (e.active ? "ACTIVE" : "SEVERED")
                << " |\n";
        }
    }
    oss << "+----------------------+----------------------+--------+----------+";
    return oss.str();
}
