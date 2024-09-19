#include <iostream>
#include "suffix_array.hpp"

using namespace std;


int main() {

    string s;
    cin >> s;

    SuffixArray ar = SuffixArray(s);
    ar.BuildArray();
    


    return 0;
}