#include <iostream>
#include <vector>
#include <queue>

using namespace std;

using graph = vector<vector<int> >;

void topology_sort(const graph &g, vector<int> &degrees, queue<int> &q, vector<int> &res) {
    
    while(!q.empty()) {
        int v = q.front();
        q.pop();
        res.push_back(v);
        // cout << "vertex: " << v << endl;
        for (size_t i = 0; i < g[v].size(); i++) {
            degrees[g[v][i]]--;
            if (degrees[g[v][i]] == 0) {
                q.push(g[v][i]);
            }
        }
    }
}

int main() {

    int n, m;
    cin >> n >> m;
    
    // будем работать в 1-индексации
    graph g(n);
    vector<int> degrees(n, 0);
    queue<int> q;
    vector<int> res;

    // рассматриваем ребра из вершины u в вершину v
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--;
        v--;
        g[u].push_back(v);
        degrees[v]++; // попутно считаем количество входящих в каждую вершину ребер 
    }

    for (size_t i = 0; i < n; i++) {
        if (degrees[i] == 0) {
            q.push(i);
        }
    }

    topology_sort(g, degrees, q, res);
    
    if (res.size() != n) {
        cout << -1 << endl;
    } else {
        for (size_t i = 0; i < res.size() - 1; i++) {
            cout << res[i] + 1 << " " << res[i + 1] + 1 << endl;
        } 
    }

    return 0;
}