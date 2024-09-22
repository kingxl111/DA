#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

vector<int> BuildSuffixArray(const string &s) {
    int n = s.size();
    vector<int> suffAr(n), eqClass(n), tmp(n);
        
    for (int i = 0; i < n; i++) {
        suffAr[i] = i;
        eqClass[i] = s[i];
    }
    
    for (int len = 1; len < n; len *= 2) {
        auto cmp = [&](int a, int b) {
            if (eqClass[a] != eqClass[b])  {
                return eqClass[a] < eqClass[b];
            }
            return (a + len < n ? eqClass[a + len] : -1) < (b + len < n ? eqClass[b + len] : -1);
        };
        
        sort(suffAr.begin(), suffAr.end(), cmp);
        
        tmp[suffAr[0]] = 0;
        for (int i = 1; i < n; i++) {
            tmp[suffAr[i]] = tmp[suffAr[i - 1]] + (cmp(suffAr[i - 1], suffAr[i]) ? 1 : 0);
        }
        eqClass.swap(tmp);
    }
    
    return suffAr;
}

vector<int> FindPattern(const string &text, const string &pattern, vector<int> &suffAr) {
    
    int m = pattern.size();
    int left = 0, right = text.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (text.substr(suffAr[mid], m) < pattern) {
            left = mid + 1;
        } 
        else {
            right = mid - 1;
        }
    }
    
    int start = left;
    right = text.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (text.substr(suffAr[mid], m) <= pattern) {
            left = mid + 1;
        } 
        else {
            right = mid - 1;
        }
    }

    int end = right;

    vector<int> indices;
    for (int i = start; i <= end; i++) {
        indices.push_back(suffAr[i]);
    }

    return indices;
}

int main() {
    cin.tie(0);
    ios::sync_with_stdio(false);

    string text;
    getline(cin, text);
    string pattern;
    int counter = 1;

    vector<int> sufAr = BuildSuffixArray(text);

    while(getline(cin, pattern)) {
        vector<int> indices = FindPattern(text, pattern, sufAr);
        sort(indices.begin(), indices.end());
        if(indices.size() == 0) {
            ++counter;
            continue;
        }
        cout << counter << ": ";
        for (int i = 0; i < indices.size(); ++i) {
            if(i == indices.size() - 1) {
                cout << indices[i] + 1;
            }
            else {
                cout << indices[i] + 1 << ", ";
            }
        }
        cout << endl;
        ++counter;
    }

    return 0;
}
