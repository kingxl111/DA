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

    
    Vector<int> ar;
    int x;
    while(cin >> x) {
        ar.push_back(x);
    }   

    // Сортируем вставкой(неважно, что за квадрат, так как N небольшое)
    for (int i = 0; i < ar.size(); ++i) {
        int x = ar[i];
        int j = 0;
        while(ar[j] < x) {
            ++j;
        }
        for (int c = i; c > j; --c) {
            ar[c] = ar[c - 1];
        }
        ar[j] = x;
    }

    for (int i = 0; i < ar.size(); ++i)
    {
        cout << ar[i] << " ";
    } 
    cout << endl;
    

    return 0;
}