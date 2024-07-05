#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream>

using namespace std;

struct TElem {
    int lineNumber;
    int wordNumber; 
    int begIdx;
};

vector<int> zF(const string &s) {
    vector<int> z(s.size());
    int n = s.size();
    int l = 0, r = 0;
    for (int i = 0; i < n; i++) {
        if (i <= r) {
            z[i] = min(z[i - l], r - i + 1);
        }
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        };
    }
    z[0] = 0;
    return z;
}

void ToLowerCase(string &s) {
    for (size_t i = 0; i < s.size(); i++)  {
        if(s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = static_cast<char>(static_cast<int>(s[i]) + static_cast<int>('a' - 'A'));
        }
    }   
}

TElem LowerBound(vector<TElem> &v, const TElem &x) {
    int l = 0;
    int r = v.size();
    while(r - l != 1) {
        int m = (l + r) / 2;
        if(v[m].begIdx < x.begIdx) {
            l = m;
        }
        else {
            r = m;
        }
    }
    if(v[l].begIdx == x.begIdx) {
        return v[l];
    }
    if(v[r].begIdx == x.begIdx) {
        return v[r];
    }
    return v[(l + r) / 2];
}

int main() {

    vector<string> ptrns;
    
    string ptrn;
    getline(cin, ptrn);
    ToLowerCase(ptrn);
    istringstream ss1(ptrn);
    string tmpStr;
    ptrn = "";
    while(ss1 >> tmpStr) {
        ptrn += tmpStr + " ";
    }
    ptrns.push_back(ptrn);

    string commonString = ""; // joined text
    vector<TElem> words;

    string curString = "";
    int lineNum = 1;
    int idx = 1;
    while(getline(cin, curString)) {
        ToLowerCase(curString);
        istringstream ss(curString);
        string curWord = "";
        int wordNum = 1;
        while(ss >> curWord) {
            commonString += curWord + " ";
            words.push_back({lineNum, wordNum, idx});
            ++wordNum;
            idx += curWord.size() + 1;
        }
        ++lineNum;
    }

    int ptrnLen = ptrn.size();
    vector<int> z = zF(ptrn + "$" + commonString);

    for (size_t j = ptrnLen; j < z.size(); ++j) {
        // cout << z[j]  << " " << j + 1 << endl;
        if(z[j] == ptrnLen) {
            int indx = j - ptrnLen;
            TElem x;
            x.begIdx = indx;
            TElem ans = LowerBound(words, x);
            cout << ans.lineNumber << ", " << ans.wordNumber << endl;
        }
    }
    
    return 0;
}
