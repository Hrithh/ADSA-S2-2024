#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
#include <limits>
#include <queue>
#include <set>

using namespace std;

const int INF = numeric_limits<int>::max();

struct Edge {
    int u, v, cost;
    bool isExisting; // True if it's an existing road
    bool operator>(const Edge& other) const {
        return cost > other.cost;
    }
};

class UnionFind {
public:
    vector<int> parent, rank;
    UnionFind(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int u) {
        if (u != parent[u]) parent[u] = find(parent[u]);
        return parent[u];
    }

    bool unite(int u, int v) {
        int rootU = find(u), rootV = find(v);
        if (rootU == rootV) return false; 
        if (rank[rootU] > rank[rootV]) {
            parent[rootV] = rootU;
        } else if (rank[rootU] < rank[rootV]) {
            parent[rootU] = rootV;
        } else {
            parent[rootV] = rootU;
            ++rank[rootU];
        }
        return true;
    }
};

int convertCost(char c) {
    return isupper(c) ? c - 'A' + 1 : c - 'a' + 27; // Adjusted costs
}

void dfs(int node, const vector<vector<int>>& country, vector<bool>& visited, vector<int>& component) {
    visited[node] = true;
    component.push_back(node);
    for (size_t neighbor = 0; neighbor < country.size(); ++neighbor) {
        if (country[node][neighbor] == 1 && !visited[neighbor]) {
            dfs(neighbor, country, visited, component);
        }
    }
}

vector<vector<int>> findComponents(const vector<vector<int>>& country) {
    size_t N = country.size();
    vector<bool> visited(N, false);
    vector<vector<int>> components;

    for (size_t i = 0; i < N; ++i) {
        if (!visited[i]) {
            vector<int> component;
            dfs(i, country, visited, component);
            components.push_back(component);
        }
    }

    return components;
}

void parseInput(const string& input, vector<vector<int>>& country, vector<vector<int>>& build, vector<vector<int>>& destroy) {
    size_t pos = 0;
    string delimiter = " ";
    vector<string> parts;
    string inputCopy = input;

    while ((pos = inputCopy.find(delimiter)) != string::npos) {
        parts.push_back(inputCopy.substr(0, pos));
        inputCopy.erase(0, pos + delimiter.length());
    }
    parts.push_back(inputCopy);

    int N = count(parts[0].begin(), parts[0].end(), ',') + 1;

    country.resize(N, vector<int>(N));
    build.resize(N, vector<int>(N));
    destroy.resize(N, vector<int>(N));

    // Fill country adjacency matrix
    for (int i = 0; i < N; ++i) {
        size_t start = 0, end;
        for (int j = 0; j < N; ++j) {
            end = parts[0].find(',', start);
            country[i][j] = stoi(parts[0].substr(start, end - start));
            start = end + 1;
        }
    }

    // Fill build and destroy costs
    for (int k = 1; k <= 2; ++k) {
        for (int i = 0; i < N; ++i) {
            size_t start = 0, end;
            for (int j = 0; j < N; ++j) {
                end = parts[k].find(',', start);
                if (k == 1) {
                    build[i][j] = convertCost(parts[k].substr(start, end - start)[0]);
                } else {
                    destroy[i][j] = convertCost(parts[k].substr(start, end - start)[0]);
                }
                start = end + 1;
            }
        }
    }
}

int calculateMinCost(const vector<vector<int>>& country, const vector<vector<int>>& build, const vector<vector<int>>& destroy) {
    vector<vector<int>> components = findComponents(country);
    int totalCost = 0;

    priority_queue<Edge, vector<Edge>, greater<Edge>> edges;

    // Push edges for existing roads (which can be destroyed)
    for (const auto& component : components) {
        for (int u : component) {
            for (int v : component) {
                if (u != v && country[u][v] == 1) {
                    edges.push({u, v, destroy[u][v], true});
                    cout << "Added existing edge from " << u << " to " << v << " with cost " << destroy[u][v] << " (existing)" << endl; // Debug statement added here
                }
            }
        }
    }

    // Push edges for potential new roads (which can be built)
    for (size_t i = 0; i < components.size(); ++i) {
        for (size_t j = i + 1; j < components.size(); ++j) {
            for (int u : components[i]) {
                for (int v : components[j]) {
                    edges.push({u, v, build[u][v], false});
                    cout << "Added new edge from " << u << " to " << v << " with cost " << build[u][v] << " (new)" << endl; // Debug output
                }
            }
        }
    }

    UnionFind uf(components.size());
    while (!edges.empty()) {
        Edge edge = edges.top();
        edges.pop();

        // Find the component indices for u and v
        int componentU = -1, componentV = -1;
        for (size_t compIndex = 0; compIndex < components.size(); ++compIndex) {
            if (find(components[compIndex].begin(), components[compIndex].end(), edge.u) != components[compIndex].end()) {
                componentU = compIndex;
            }
            if (find(components[compIndex].begin(), components[compIndex].end(), edge.v) != components[compIndex].end()) {
                componentV = compIndex;
            }
        }

        // Process the edge
        cout << "Processing edge from " << edge.u << " to " << edge.v 
             << " with cost " << edge.cost << " (isExisting: " << edge.isExisting << ")" << endl;

        // Unite components if they are different
        if (componentU != componentV && uf.unite(componentU, componentV)) {
            totalCost += edge.cost; // Add cost to total only if a union is made
            cout << "Union successful. Total cost updated to: " << totalCost << endl;
        }
    }

    return totalCost;
}

int main() {
    string input;
    getline(cin, input);

    vector<vector<int>> country, build, destroy;
    parseInput(input, country, build, destroy);

    int result = calculateMinCost(country, build, destroy);

    cout << result << endl;
    return 0;
}
