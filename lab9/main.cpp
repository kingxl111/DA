#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <queue>

using namespace std;

using graph = vector<vector<int>>;

bool isBipartiteBFS(graph &g, vector<int> &color) {
    int n = g.size();

    for (int start = 0; start < n; ++start) {
        if (color[start] == -1) {
            queue<int> q;
            q.push(start);
            color[start] = 0;

            while (!q.empty()) {
                int v = q.front();
                q.pop();

                for (int u : g[v]) {
                    if (color[u] == -1) {
                        color[u] = 1 - color[v];
                        q.push(u);
                    } 
                    else if (color[u] == color[v]) { // already visited
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool findIncChain(graph &g, vector<bool> &visited, vector<int> &match, int v) {
    if (visited[v]) {
        return false;
    }
    visited[v] = true;

    for (int u : g[v]) {
        if (match[u] == -1 || findIncChain(g, visited, match, match[u])) {
            match[v] = u;
            match[u] = v;
            return true;
        }
    }
    return false;
}

set<pair<int, int>> Kuhn(graph &g, const vector<int> &left_partition) {
    int n = g.size();
    vector<int> match(n, -1);
    set<pair<int, int>> matchmaking;

    for (int v : left_partition) {
        vector<bool> visited(n, false);
        if (match[v] == -1) {
            findIncChain(g, visited, match, v);
        }
    }

    for (size_t i = 0; i < n; i++) {
        if (match[i] != -1) {
            matchmaking.insert({min(int(i), match[i]), max(int(i), match[i])});
        }
    }
    return matchmaking;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n, m;
    cin >> n >> m;

    graph g(n);
    for (size_t i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        --u; // to 0-indexing
        --v; // to 0-indexing
        g[u].push_back(v);
        g[v].push_back(u);
    }

    for (size_t i = 0; i < g.size(); i++) {
        sort(g[i].begin(), g[i].end());
    }
    

    vector<int> color(n, -1);
    if (!isBipartiteBFS(g, color)) {
        cout << 0 << endl;
        return 0;
    }

    vector<int> left_partition;
    for (int i = 0; i < n; ++i) {
        if (color[i] == 0) {
            left_partition.push_back(i);
        }
    }

    set<pair<int, int>> ans = Kuhn(g, left_partition);
    cout << ans.size() << endl;
    for (const auto &p : ans) {
        cout << p.first + 1 << " " << p.second + 1 << endl; // to 1-indexing
    }

    return 0;
}
