#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <tuple>
#include <algorithm>
#include <climits>      
#include <functional>   

using namespace std;

// Convert letter to cost
int letter_to_cost(char letter) {
    if (letter >= 'A' && letter <= 'Z') {
        return letter - 'A';
    } else if (letter >= 'a' && letter <= 'z') {
        return letter - 'a' + 26;
    }
    return INT_MAX;  
}

// Parse input strings into 2D matrices and convert to integer costs 
void parse_input(const string &input, vector<vector<int>> &country, vector<vector<int>> &build, vector<vector<int>> &destroy) {
    auto parse_matrix = [](const string &matrix_str, int n, bool convert = false) {
        vector<vector<int>> matrix(n, vector<int>(n, 0));
        int idx = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                matrix[i][j] = convert ? letter_to_cost(matrix_str[idx++]) : matrix_str[idx++] - '0';
            }
            idx++; 
        }
        return matrix;
    };

    int first_space = input.find(' ');
    int second_space = input.find(' ', first_space + 1);

    int n = count(input.begin(), input.end(), ',') / 3 + 1;
    country = parse_matrix(input.substr(0, first_space), n, false);
    build = parse_matrix(input.substr(first_space + 1, second_space - first_space - 1), n, true);
    destroy = parse_matrix(input.substr(second_space + 1), n, true);
}

// Find connected components in the initial city 
void find_components(const vector<vector<int>> &country, vector<vector<int>> &components, vector<int> &component_id) {
    int n = country.size();
    vector<bool> visited(n, false);
    int component_count = 0;

    function<void(int)> dfs = [&](int node) {
        visited[node] = true;
        components[component_count].push_back(node);
        component_id[node] = component_count;
        for (int neighbor = 0; neighbor < n; ++neighbor) {
            if (country[node][neighbor] == 1 && !visited[neighbor]) {
                dfs(neighbor);
            }
        }
    };

    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            components.emplace_back();
            dfs(i);
            component_count++;
        }
    }
}

// Union-Find structure for Kruskal's MST
struct UnionFind {
    vector<int> parent, rank;

    UnionFind(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }

    bool union_sets(int x, int y) {
        int rootX = find(x), rootY = find(y);
        if (rootX != rootY) {
            if (rank[rootX] > rank[rootY]) parent[rootY] = rootX;
            else if (rank[rootX] < rank[rootY]) parent[rootX] = rootY;
            else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
            return true;
        }
        return false;
    }
};

// Calculate minimum cost to reconstruct the road network
int calculate_min_cost(const vector<vector<int>> &country, const vector<vector<int>> &build, const vector<vector<int>> &destroy) {
    int n = country.size();
    vector<tuple<int, int, int, bool>> edges;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (country[i][j] == 1) {
                edges.emplace_back(destroy[i][j], i, j, false); 
            } else {
                edges.emplace_back(build[i][j], i, j, true);  
            }
        }
    }

    sort(edges.begin(), edges.end());
  
    UnionFind uf(n);
    int total_cost = 0;

    for (const auto &edge : edges) {
        int cost, u, v;
        bool is_build;
        tie(cost, u, v, is_build) = edge;
        
        if (uf.union_sets(u, v)) {
            total_cost += cost;
        }
    }

    return total_cost;
}

int main() {
    string input_string = "011,101,110 ABD,BAC,DCA ABD,BAC,DCA"; 
    vector<vector<int>> country, build, destroy;

    parse_input(input_string, country, build, destroy);

    vector<vector<int>> components;
    vector<int> component_id(country.size(), -1);
    find_components(country, components, component_id);

    int min_cost = calculate_min_cost(country, build, destroy);
    cout << min_cost << endl;

    return 0;
}
