#include <iostream>
#include "suffix_array.hpp"

using namespace std;


int main() {

    string text;
    cin >> text;

    SuffixArray ar = SuffixArray(text);
    ar.BuildArray();
    
    string pattern;
    int i = 1;
    while(cin >> pattern) {
        vector<int> pos = ar.Find(pattern);
        cout << i << ": ";
        for (size_t j = 0; j < pos.size(); ++j) {
            if(j == pos.size() - 1) {
                cout << pos[j] + 1;
            
            } else {
                cout << pos[j] + 1 << ", ";
            }
        } cout << endl;
        ++i;
    }


    return 0;
}