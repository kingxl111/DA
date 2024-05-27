#include <bits/stdc++.h>
#include "Aho–Corasick.hpp"

using namespace std;
// One-to-many string pattern matching algorithm
// Aho-Corasick
// String length N, k pattern, each length m [i] (1 <= i <= k)
int main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(0);

    int N, M;
    Trie* root = new Trie;

    cin >> N;
    buildTrieAndFailureLinks(root, N);

    cin >> M;
    for(int i = 0; i < M; i++) {
        char str[10001];
        cin >> str;
        cout << (searchInTrie(root, str) ? "YES" : "NO") << "\n";
    }

    delete root;
}