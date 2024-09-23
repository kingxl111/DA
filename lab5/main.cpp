#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <map>

using namespace std;

class SuffixArray {
private:

    struct Item {
        int idx; // индекс начала подстроки
        pair<int, int> oldEqClass; // переходы осуществляем по первому элементу
        int newEqClass;
    };

    int textSize; // Размер текста с учетом sentinel, округленный вверх до степени двойки
    string text;
    vector<Item> array;
    char sentinel = '$';
    vector<int> idxEqClass;

    
    int Modulo(int, int);
    void CountingSortPair(vector<pair<int, int> > &array);
    void CountingSortItem(vector<Item> &array);

public:

    SuffixArray(const string &t) {        

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

        for (size_t i = 0; i < textSize + 1; ++i){
            idxEqClass.push_back(0);
        }

        vector<pair<int, int> > letterIdx(text.size());
        for (size_t i = 0; i < letterIdx.size(); ++i) {
            if(text[i] == sentinel) {
                letterIdx[i] = {0, i};
                continue;
            }
            letterIdx[i] = {text[i] - static_cast<int>('a') + 1, i};
        }

        CountingSortPair(letterIdx);

        /*
        for (size_t i = 0; i < letterIdx.size(); ++i) {
            cout << "Letter: " <<  (char)(letterIdx[i].first + 'a' - 1) << ", idx: " << letterIdx[i].second << endl;
        }
        */
        
        // Сначала заполняем индексы отсортированных литер 
        for (size_t i = 0; i < textSize; ++i) {
            Item curItem;
            curItem.idx = letterIdx[i].second;
            array.push_back(curItem);
        }

        // Теперь заполняем классы эквивалентности
        // Про вторую пару класса эквивалентности пока ничего не знаем
        array[0].oldEqClass.first = 0;
        idxEqClass[array[0].idx] = array[0].oldEqClass.first;
        for (size_t i = 1; i < textSize; ++i) {
            if(text[array[i].idx] == text[array[i - 1].idx]) {
                array[i].oldEqClass.first = array[i - 1].oldEqClass.first;
            } else {
                array[i].oldEqClass.first = array[i - 1].oldEqClass.first + 1;
            }
            idxEqClass[array[i].idx] = array[i].oldEqClass.first;
        }
        // for (size_t i = 0; i < textSize; ++i) {
        //     cout << "idx: " << array[i].idx << ", eq_class: " << array[i].oldEqClass.first << endl;
        // }
    } 

    void BuildArray() {

        for(int i = 1; i < textSize; i *= 2) {
            
            for(int j = 0; j < textSize; ++j) {
                int newIdx = Modulo(array[j].idx - i, textSize);
                array[j].newEqClass = idxEqClass[newIdx]; // Перекладываем класс эквивалентности
                // cout << "idx: " << j << ", newIdx: " << newIdx << ", newEq: "
                // << idxEqClass[newIdx] << endl;
            }

            for(int j = 0; j < textSize; ++j) {
                int newIdx = Modulo(array[j].idx - i, textSize);
                array[j].idx = newIdx;
                array[j].oldEqClass.first = array[j].newEqClass; // По этим классам будем сортировать
            }

            // Сортируем по классу эквивалентности 
            CountingSortItem(this->array);

            for(int j = 0; j < textSize; ++j) {
                array[j].oldEqClass.second = idxEqClass[Modulo(array[j].idx + i, textSize)];
            }

            // Строим новые классы эквивалентности
            array[0].newEqClass = 0;
            for(int j = 1; j < textSize; ++j) {
                if((array[j].oldEqClass.first == array[j - 1].oldEqClass.first) 
                 && (array[j].oldEqClass.second == array[j - 1].oldEqClass.second)) {
                    array[j].newEqClass = array[j - 1].newEqClass;
                } else {
                    array[j].newEqClass = array[j - 1].newEqClass + 1;
                }
            }
            
            /*
            for (int k = 0; k < textSize; ++k) {
                cout << "idx: " << array[k].idx << ", old_eq_first: " << array[k].oldEqClass.first 
                << ", old_eq_sec: " << array[k].oldEqClass.second << ", new_eq: " << 
                array[k].newEqClass << endl;
            }
            */

            for (int j = 0; j < textSize; ++j) {
                idxEqClass[array[j].idx] = array[j].newEqClass;
                array[j].oldEqClass.first = array[j].newEqClass;
            }

            /*
            for (int k = 0; k < textSize; ++k) {
                cout << "idx: " << array[k].idx << ", idxEqClass " 
                << idxEqClass[array[k].idx]  << endl;
            }
            */

            if (array[textSize - 1].newEqClass == textSize - 1) {
                // cout << "end string len: " << i << endl;
                break;
            }
        }
    }

    vector<int> Find(const string &pattern) {

        int m = pattern.size();
        int left = 0, right = text.size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (text.substr(array[mid].idx, m) < pattern) {
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
            if (text.substr(array[mid].idx, m) <= pattern) {
                left = mid + 1;
            } 
            else {
                right = mid - 1;
            }
        }

        int end = right;

        vector<int> indices;
        for (int i = start; i <= end; i++) {
            indices.push_back(array[i].idx);
        }

        sort(indices.begin(), indices.end());

        return indices;
    }

    ~SuffixArray() {}

};

int SuffixArray::Modulo(int x, int y) {
    if (x % y == 0) {
        return 0;
    } 
    else {

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

void SuffixArray::CountingSortPair(vector<pair<int, int> > &ar) {

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

    ar.swap(newAr);
}

void SuffixArray::CountingSortItem(vector<Item> &ar) {

    vector<Item> newAr(ar.size());

    int maxim = 0;
    for (size_t i = 0; i < ar.size(); ++i) {
        maxim = max(maxim, ar[i].oldEqClass.first);
    }
    int count[maxim + 1];
    for(int k = 0; k <= maxim; ++k)  {
        count[k] = 0;
    }

    for(int i = 0; i < ar.size(); ++i)  {
        int idx = ar[i].oldEqClass.first;
        ++count[idx];
    }

    int prev = 0;
    for(int k = 0; k <= maxim; ++k) {
        count[k] += prev;
        prev = count[k];
    }

    for(int i = ar.size() - 1; i >= 0; --i)  {
        int idxInCount = ar[i].oldEqClass.first;
        int idxInResult = count[idxInCount] - 1;
        newAr[idxInResult] = ar[i];
        --count[idxInCount];
    }

    ar.swap(newAr);
}



int main() {

    cin.tie(0);
    ios::sync_with_stdio(false);

    string text;
    cin >> text;

    SuffixArray ar = SuffixArray(text);
    ar.BuildArray();
    
    string pattern;
    int i = 1;
    while(cin >> pattern) {
        vector<int> pos = ar.Find(pattern);
        if(pos.size() == 0) {
            ++i;
            continue;
        }
        cout << i << ": ";
        for (size_t j = 0; j < pos.size(); ++j) {
            if(j == pos.size() - 1) {
                cout << pos[j] + 1;
            
            } else {
                cout << pos[j] + 1 << ", ";
            }
        } 
        cout << endl;
        ++i;
    }


    return 0;
}