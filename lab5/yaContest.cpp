#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Функция для построения суффиксного массива
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

// Функция для вычисления массива LCP
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
            while (i + h < n && j + h < n && s[i + h] == s[j + h]) h++;
            lcp[eqClass[i]] = h;
            if (h > 0) h--;
        }
    }
    
    return lcp;
}

// Функция для поиска паттерна
vector<int> FindPattern(const string &text, const string &pattern) {
    vector<int> suffAr = BuildSuffixArray(text);
    vector<int> lcp = BuildLcp(text, suffAr);
    
    int m = pattern.size();
    int left = 0, right = text.size() - 1;
    
    // Бинарный поиск для нахождения первого вхождения
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (text.substr(suffAr[mid], m) < pattern) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    int start = left;

    // Бинарный поиск для нахождения последнего вхождения
    right = text.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (text.substr(suffAr[mid], m) <= pattern) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    int end = right;

    // Извлечение индексов вхождений
    vector<int> indices;
    for (int i = start; i <= end; i++) {
        indices.push_back(suffAr[i]);
    }

    return indices;
}

int main() {
    string text;
    cin >> text;
    string pattern;
    int counter = 1;
    while(cin >> pattern) {
        vector<int> indices = FindPattern(text, pattern);
        sort(indices.begin(), indices.end());
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
