#include <iostream>
#include "vector.hpp"

using std::cin;
using std::cout;
using std::endl;
using vector::Vector;

int bin_search(Vector<int>& keys, int k) {
    int l = -1;
    int r = keys.size();

    int idx = -1;
    while(r - l != 1) {
        int m = (l + r) / 2;
        if(keys[m] < k) {
            l = m;
        }
        else if (keys[m] > k) {
            r = m;
        }
        else {
            idx = m;
            return idx;
        }
    }
    if(keys[l] == k) {
        return l;
    }
    if(keys[r] == k) {
        return r;
    }
    return idx;
}

int main() {

    
    
    

    return 0;
}