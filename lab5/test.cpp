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

vector<int> BuildLcp(const string &s, const vector<int> &suffAr) {
    int n = s.size();
    vector<int> eqClass(n), lcp(n);
    
    for (int i = 0; i < n; i++) {
        eqClass[suffAr[i]] = i;
    }
    
    int h = 0;
    for (int i = 0; i < n; i++) {
        if (eqClass[i] > 0) {
            int j = suffAr[eqClass[i] - 1];

            while (i + h < n && j + h < n && s[i + h] == s[j + h]) {
                h++;
            }
            lcp[eqClass[i]] = h;

            if (h > 0) {
                h--;
            }
        }
    }
    
    return lcp;
}

vector<int> FindPattern(const string &text, const string &pattern) {
    vector<int> suffAr = BuildSuffixArray(text);
    vector<int> lcp = BuildLcp(text, suffAr);
    
    /*
    for (size_t i = 0; i < lcp.size(); ++i ) {
        cout << "SuffArray[" << i << "]: " << suffAr[i] << ", lcp[" << i << "]: " <<  lcp[i] << endl;
    }
    */
    
    int left = 0; 
    int right = text.size();   
    bool entering = false;

    while(right - left > 1) {
        cout << left << " " << right << endl;
        int m = (left + right) / 2;
        string s1;
        
        for (int i = 0; i + suffAr[m] < text.size(); ++i) {
            s1 += text[suffAr[m] + i];
        }
        
        int i = 0, j = 0;
        while(i < pattern.size() && j < s1.size()) {
            if(pattern[i] > s1[j]) {
                left = m + 1;
                break;
            }
            else if (pattern[i] < s1[j]) {
                right = m - 1;
                // cout << "pattern[i] < s1[j], right = " << right << endl;
                // cout << "right = m - 1" << endl;
                break;
            } 
            ++i;
            ++j;
        }
        cout << pattern << " " <<  s1 << endl;
        cout << "l = " << left << ", r = " << right << endl;
        if(i == pattern.size()) {
            entering = true;
            if(lcp[m] >= i) {
                right = m;
            }
            else {
                left = m;
            }
        }
        else if(j == s1.size()) {
            left = m + 1;
        }
    } 
    vector<int> indices;

    int L = left;
    if()
    if(!entering) {
        return indices;
    }

    // cout << "left: " << left << ", right:  " << right << endl;


    indices.push_back(suffAr[L]);
    for (int idx = L + 1; idx < lcp.size(); ++idx) {
        if(lcp[idx] >= pattern.size()) {
            indices.push_back(suffAr[idx]);
        }
        else {
            break;
        }
    }
    

    return indices;
}

int main() {
    string text;
    getline(cin, text);
    string pattern;
    int counter = 1;
    while(getline(cin, pattern)) {
        vector<int> indices = FindPattern(text, pattern);
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
