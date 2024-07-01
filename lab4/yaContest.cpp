#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct TElem {
    uint16_t wordIdx;
    uint16_t lineNumber;
    uint16_t wordNumber; 
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

bool cmp(TElem el1, TElem el2) {
    return el1.wordIdx < el2.wordIdx;
}

TElem LowerBound(vector<TElem> &v, const TElem &x) {
    int l = 0;
    int r = v.size();
    while(r - l != 1) {
        int m = (l + r) / 2;
        if(v[m].wordIdx < x.wordIdx) {
            l = m;
        }
        else {
            r = m;
        }
    }
    if(v[l].wordIdx == x.wordIdx) {
        return v[l];
    }
    if(v[r].wordIdx == x.wordIdx) {
        return v[r];
    }
    return v[(l + r) / 2];
}

int main() {

    ios::sync_with_stdio(false);
    cin.tie(0);

    vector<string> ptrns;

    string ptrn;
    do {
        getline(cin, ptrn);
        ToLowerCase(ptrn);
        ptrns.push_back(ptrn);
        
    } while(ptrn != "") ;
    ptrns.pop_back();

    string commonString = ""; // joined text

    uint16_t idx = 1;
    int c;
    uint16_t lineNum = 1;

    vector<TElem> words;
    while((c = getchar()) != EOF) {
        string curString = "";
        uint16_t curWordBeginning = idx;
        uint16_t wordNumber = 1;
        while((c != '\n') && (c != EOF)) {
            curString += c;
            ++idx;
            if(c == ' ') {
                words.push_back({curWordBeginning, lineNum, wordNumber});
                wordNumber++;
                curWordBeginning = idx;
            }
            c = getchar();
        }
        curString += ' ';
        words.push_back({curWordBeginning, lineNum, wordNumber});
        ToLowerCase(curString);
        commonString += curString;
        lineNum++;
    }

    // for (size_t i = 0; i < words.size(); i++) {
    //     cout << words[i].wordIdx << " " << words[i].lineNumber << " " << words[i].wordNumber << endl;
    // }
    

    // for (size_t i = 0; i < words.size(); i++) {
    //     for (size_t j = 0; j < words[i].size(); j++) {
    //         cout << words[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    
    // for (size_t i = 0; i < commonString.size(); i++) {
    //     cout << commonString[i] << " " << i + 1 << endl;
    // }


    if(ptrns.size() == 1) {

        int ptrnLen = ptrns[0].size();
        vector<int> z = zF(ptrns[0] + "$" + commonString);

        for (int j = 0; j < z.size(); ++j) {
            // cout << z[j]  << " " << j + 1 << endl;
            if(z[j] == ptrnLen) {
                int indx = j - ptrnLen;
                TElem x;
                x.wordIdx = indx;
                TElem ans = LowerBound(words, x);
                cout << ans.lineNumber << ", " << ans.wordNumber << endl;

            }
        }
    }
    else {

        for (size_t i = 0; i < ptrns.size(); i++) {
            int ptrnLen = ptrns[i].size();
            vector<int> z = zF(ptrns[i] + "$" + commonString);

            for (int j = 0; j < z.size(); ++j) {
                // cout << z[j]  << " " << j + 1 << endl;
                if(z[j] == ptrnLen) {
                    int indx = j - ptrnLen;
                    TElem x;
                    x.wordIdx = indx;
                    TElem ans = LowerBound(words, x);
                    cout << ans.lineNumber << ", " << ans.wordNumber  << ", " << i + 1 << endl;

                }
            }
        }
    
    }


    /*
    string text, ptrn;
    getline(cin, text);
    getline(cin, ptrn);
    vector<int> z = zF(ptrn + "$" + text);

    for (int i = 0; i < z.size(); i++) {
        cout << z[i] << " ";
    } cout << endl;

    for (int i = 0; i < z.size(); ++i) {
        if(z[i] == ptrn.size()) {
            cout << i - ptrn.size() - 1 << endl;
        }
    }
    */
    
    return 0;
}
