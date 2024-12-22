#include <iostream>
#include <vector>

using namespace std;

int LCS(const string &s1, const string &s2) {
    
    vector<vector<int> > lcs(s1.size() + 1);
    for (size_t i = 0; i < s1.size() + 1; i++) {
        vector<int> row(s2.size() + 1);
        for (size_t j = 0; j < s2.size() + 1; j++) {
            row[j] = 0;
        }
        lcs[i] = row;
    }

    for (size_t i = 1; i < s1.size() + 1; i++) {
        for (size_t j = 1; j < s2.size() + 1; j++) {
            if(s1[i - 1] == s2[j - 1]) {
                lcs[i][j] = lcs[i - 1][j - 1] + 1;
            }
            else {
                lcs[i][j] = max(lcs[i - 1][j], lcs[i][j - 1]);
            }
        }
    }
    
    return lcs[s1.size()][s2.size()];
}

int main() {

    string s1, s2;
    cin >> s1 >> s2;

    cout << LCS(s1, s2) << endl;

    return 0;
}