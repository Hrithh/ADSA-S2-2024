#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

using namespace std;

// Helper function to convert cost character to integer
int charToCost(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';      // 0 to 25
    if (c >= 'a' && c <= 'z') return c - 'a' + 26; // 26 to 51
    return -1; // Error case
}

// Function to parse the input
void parseInput(string input, vector<vector<int>>& country, vector<vector<int>>& build, vector<vector<int>>& destroy) {
    size_t pos = 0;
    string token;
    vector<string> tokens;

    // Split input by spaces
    while ((pos = input.find(' ')) != string::npos) {
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + 1);
    }
    tokens.push_back(input); // last token

    // Parse country, build, and destroy arrays
    for (int i = 0; i < 3; ++i) {
        size_t rowPos = 0;
        vector<vector<int>> matrix;
        while ((rowPos = tokens[i].find(',')) != string::npos) {
            string row = tokens[i].substr(0, rowPos);
            vector<int> rowData;
            for (char c : row) {
                rowData.push_back(c - '0'); // Convert '0'/'1' to integer for country
            }
            matrix.push_back(rowData);
            tokens[i].erase(0, rowPos + 1);
        }
        // Last row
        if (!tokens[i].empty()) {
            vector<int> rowData;
            for (char c : tokens[i]) {
                rowData.push_back(c - '0'); // Convert '0'/'1' to integer for country
            }
            matrix.push_back(rowData);
        }

        // Assign to the appropriate matrix
        if (i == 0) {
            country = matrix;
        } else if (i == 1) {
            for (const auto& row : matrix) {
                vector<int> costRow;
                for (char c : row) {
                    costRow.push_back(charToCost(c)); // Convert costs from characters
                }
                build.push_back(costRow);
            }
        } else {
            for (const auto& row : matrix) {
                vector<int> costRow;
                for (char c : row) {
                    costRow.push_back(charToCost(c)); // Convert costs from characters
                }
                destroy.push_back(costRow);
            }
        }
    }
}

// Structure to hold edges
struct Edge {
    int u, v;
    int cost; // Cost of the edge (for building or destroying)
};

// Union-Find (Disjoint Set Union) for Kruskal's algorithm
class UnionFind {
public:
    UnionFind(int n) : parent(n), rank(n, 0) {
        iota(parent.begin(), parent.end(), 0); // Initialize parent
    }

    int find(int u) {
        if (u != parent[u]) {
            parent[u] = find(parent[u]); // Path compression
        }
        return parent[u];
    }

    void unionSets(int u, int v) {
        int rootU = find(u);
        int rootV = find(v);
        if (rootU != rootV) {
            if (rank[rootU] < rank[rootV]) {
                parent[rootU] = rootV;
            } else if (rank[rootU] > rank[rootV]) {
                parent[rootV] = rootU;
            } else {
                parent[rootV] = rootU;
                rank[rootU]++;
            }
        }
    }

private:
    vector<int> parent;
    vector<int> rank;
};

// Function to implement Kruskal's algorithm and compute the minimal cost
int kruskalAlgorithm(const vector<vector<int>>& country, const vector<vector<int>>& build, const vector<vector<int>>& destroy) {
    int n = country.size();
    vector<Edge> edges;

    // Add existing roads (with destroy costs)
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            if (country[u][v] == 1) { // Existing road
                edges.push_back({u, v, charToCost(destroy[u][v])}); // Cost to destroy
            } else {
                edges.push_back({u, v, charToCost(build[u][v])}); // Cost to build
            }
        }
    }

    // Sort edges by cost
    sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.cost < b.cost;
    });

    UnionFind uf(n);
    int totalCost = 0;

    // Process edges
    for (const auto& edge : edges) {
        int rootU = uf.find(edge.u);
        int rootV = uf.find(edge.v);
        if (rootU != rootV) {
            totalCost += edge.cost; // Add cost for either building or destroying
            uf.unionSets(edge.u, edge.v); // Union the sets
        }
    }

    return totalCost;
}

int main() {
    string input;
    getline(cin, input); // Read input line

    // Parse input
    vector<vector<int>> country, build, destroy;
    parseInput(input, country, build, destroy);

    // Handle edge cases: no cities or one city
    if (country.empty() || country.size() == 1) {
        cout << 0 << endl;
        return 0;
    }

    // Compute minimal cost
    int minCost = kruskalAlgorithm(country, build, destroy);

    // Output result
    cout << minCost << endl;

    return 0;
}
