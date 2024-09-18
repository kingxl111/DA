#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;


void CountingSort(vector<pair<int, int> > &ar) {

    vector<pair<int, int> > newAr(ar.size());

    int maxim = 0;
    for (size_t i = 0; i < ar.size(); ++i) {
        maxim = max(maxim, ar[i].first);
    }
    int count[maxim + 1];
    for(int k = 0; k <= maxim; ++k)  {
        count[k] = 0;
    }

    for(int i = 0; i < ar.size(); ++i)  {
        int idx = ar[i].first;
        ++count[idx];
    }

    int prev = 0;
    for(int k = 0; k <= maxim; ++k) {
        count[k] += prev;
        prev = count[k];
    }

    for(int i = ar.size() - 1; i >= 0; --i)  {
        int idxInCount = ar[i].first;
        int idxInResult = count[idxInCount] - 1;
        newAr[idxInResult] = ar[i];
        --count[idxInCount];
    }

    ar = newAr;
}

class SuffixArray {
private:

    struct Item {
        int idx; // индекс начала подстроки
        pair<int, int> oldEqClass; // переходы осуществляем по первому элементу
        int newEqClass;
    };

    int textSize;
    string text;
    vector<Item> array;
    char sentinel = '$';

    int Modulo(int x, int y) {
        if (x % y == 0) {
            return 0;
        } else {

            if ((x > 0 && y > 0) || (x < 0 && y < 0)) {
                return x % y;
            }
            if (x > 0 && y < 0) {
                return x % y - abs(y);
            }

            if (x < 0 && y > 0) {
                return x % y + y;
            } else {
                return 0;
            }
        }
    }

public:
    SuffixArray(string &t) {
        text = t;
        text += sentinel;
        float lg = log2(static_cast<float>(text.size()));
        lg = ceil(lg);
        size_t i = text.size();
        int newSize = pow(2, lg);
        while(i < newSize) {
            text += sentinel;
            ++i;
        }
        textSize = newSize;

        vector<pair<int, int> > letterIdx(text.size());
        for (size_t i = 0; i < letterIdx.size(); ++i) {
            if(text[i] == sentinel) {
                letterIdx[i] = {0, i};
                continue;
            }
            letterIdx[i] = {text[i] - static_cast<int>('a') + 1, i};
        }

        CountingSort(letterIdx);

        // for (size_t i = 0; i < letterIdx.size(); ++i) {
        //     cout << "Letter: " <<  letterIdx[i].first + 'a' << ", idx: " << letterIdx[i].second << endl;
        // }
        
        // Сначала заполняем индексы отсортированных литер 
        for (size_t i = 0; i < textSize; ++i) {
            Item curItem;
            curItem.idx = letterIdx[i].second;
            array.push_back(curItem);
        }

        // Теперь заполняем классы эквивалентности
        // Про вторую пару класса эквивалентности пока ничего не знаем
        array[0].oldEqClass.first = 0;
        for (size_t i = 1; i < textSize; ++i) {
            if(text[array[i].idx] == text[array[i - 1].idx]) {
                array[i].oldEqClass.first = array[i - 1].oldEqClass.first;
            } else {
                array[i].oldEqClass.first = array[i - 1].oldEqClass.first + 1;
            }
        }
        // for (size_t i = 0; i < textSize; ++i) {
        //     cout << "idx: " << array[i].idx << ", eq_class: " << array[i].oldEqClass.first << endl;
        // }
    } 

    void BuildArray() {}

    string Get() {
        return text;
    }


    ~SuffixArray() {}

};


