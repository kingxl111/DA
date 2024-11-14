
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <map>

using namespace std;

void CountingSortPair(vector<pair<int, int> > &ar) {

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

class SuffixArray {
private:

    struct Item {
        int idx;
        pair<int, int> oldEqClass; 
        int newEqClass;
    };

    int textSize; 
    int sz; // without metadata
    string text;
    vector<Item> array;
    char sentinel = '$';
    vector<int> idxEqClass;

    
    int Modulo(int, int);
    void CountingSortItem(vector<Item> &array);

public:

    vector<int> GetArray() {
        vector<int> arr;
        for (int i = array.size() - sz + 1; i < array.size(); i++) {
            if (i == array.size() - sz + 1) {
                arr.push_back(array[0].idx);
                continue;
            }
            arr.push_back(array[i].idx);
        }
        return arr;
    }

    SuffixArray(const string &t) {        

        text = t;
        text += sentinel;
        sz = t.size() + 1;
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

        for (size_t i = 0; i < textSize; ++i) {
            Item curItem;
            curItem.idx = letterIdx[i].second;
            array.push_back(curItem);
        }

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
    } 

    void BuildArray() {

        for(int i = 1; i < textSize; i *= 2) {
            
            for(int j = 0; j < textSize; ++j) {
                int newIdx = Modulo(array[j].idx - i, textSize);
                array[j].newEqClass = idxEqClass[newIdx];
            }

            for(int j = 0; j < textSize; ++j) {
                int newIdx = Modulo(array[j].idx - i, textSize);
                array[j].idx = newIdx;
                array[j].oldEqClass.first = array[j].newEqClass;
            }

            CountingSortItem(this->array);

            for(int j = 0; j < textSize; ++j) {
                array[j].oldEqClass.second = idxEqClass[Modulo(array[j].idx + i, textSize)];
            }

            array[0].newEqClass = 0;
            for(int j = 1; j < textSize; ++j) {
                if((array[j].oldEqClass.first == array[j - 1].oldEqClass.first) 
                 && (array[j].oldEqClass.second == array[j - 1].oldEqClass.second)) {
                    array[j].newEqClass = array[j - 1].newEqClass;
                } else {
                    array[j].newEqClass = array[j - 1].newEqClass + 1;
                }
            }

            for (int j = 0; j < textSize; ++j) {
                idxEqClass[array[j].idx] = array[j].newEqClass;
                array[j].oldEqClass.first = array[j].newEqClass;
            }
            if (array[textSize - 1].newEqClass == textSize - 1) {
                break;
            }
        }
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

void BWT(string &input, vector<char> &res) {
    size_t n = input.size();
    vector<string> shifts(n);
    
    SuffixArray ar = SuffixArray(input);
    ar.BuildArray();
    vector<int> v = ar.GetArray();

    for (size_t i = 0; i < v.size(); i++) {
        res[i] = input[(v[i] + v.size() - 1) % v.size()]; // last symbols of sorted shifts
    }

}

void MTF(vector<char> &v, vector<int> &res, vector<char> alf) {
    vector<char> tmp_alf(alf.size());
    for (size_t i = 0; i < v.size(); i++) {
        int idx;
        for (size_t j = 0; j < alf.size(); j++) {
            if (v[i] == alf[j]) {
                idx = j;
                break;
            }
        }
        res[i] = idx;
        tmp_alf[0] = alf[idx];
        for (size_t j = 1; j <= idx; j++) {
            tmp_alf[j] = alf[j - 1];
        }
        for (size_t j = idx + 1; j < tmp_alf.size(); j++) {
            tmp_alf[j] = alf[j];
        }

        alf.swap(tmp_alf);
    }
}

void RLE(vector<int> &v, vector<pair<int, int>> &res) {
    int counter = 1;
    for (size_t i = 1; i < v.size(); i++) {
        if (v[i] == v[i - 1]) {
            counter++;
        }
        else {
            res.push_back({counter, v[i - 1]});
            counter = 1;
        }
    }
    res.push_back({counter, v[v.size() - 1]});
}


void ReverseRLE(vector<pair<int, int> > &codes, vector<int> &res) {
    for (size_t i = 0; i < codes.size(); i++) {
        for (size_t j = 0; j < codes[i].first; j++) {
            res.push_back(codes[i].second);
        }
    }
}

void ReverseMTF(vector<int> &v, vector<char> &res, vector<char> alf) {
    vector<char> tmp_alf(alf.size());
    for (size_t i = 0; i < v.size(); i++) {
        int idx = v[i];
        res.push_back(alf[idx]);

        tmp_alf[0] = alf[idx];
        for (size_t j = 1; j <= idx; j++) {
            tmp_alf[j] = alf[j - 1];
        }
        for (size_t j = idx + 1; j < tmp_alf.size(); j++) {
            tmp_alf[j] = alf[j];
        }

        alf.swap(tmp_alf);
    }
    
}


void ReverseBWT(vector<char> &v, string &res, const vector<char> &alf) {

    vector<pair<int, int> > vInt(v.size()); // first - letter, second - idx in origin array
    for (size_t i = 0; i < vInt.size(); i++) {
        if (v[i] == '$') {
            vInt[i] = {0, i};
            continue;
        }
        vInt[i] = {static_cast<int>(v[i] - 'a' + 1), i};
    }
    CountingSortPair(vInt);

    int beginIdx = -1;
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == '$') {
            beginIdx = i;
        }
    }

    int idx = beginIdx;
    while(res.size() < v.size() - 1) { // without terminal $, so -1 
        res += v[vInt[idx].second];
        idx = vInt[idx].second;
    }

}

int main() {

    ios::sync_with_stdio(false);
    cin.tie(0);

    // alf init: $ -> 0, a -> 1, b -> 2, ... , z -> 26
    vector<char> alf(27);
    alf[0] = '$';
    for (size_t i = 1; i < alf.size(); i++) {
        alf[i] = static_cast<char>('a' + i - 1);
    }
    
    string cmd;
    cin >> cmd;
    if (cmd == "compress") {
        string inputText;
        cin >> inputText;
        inputText += "$"; // for easier BWT decoding 
        
        vector<char> bwt(inputText.size());
        BWT(inputText, bwt);

        vector<int> mtf(bwt.size());
        MTF(bwt, mtf, alf);

        vector<pair<int, int> > rle;
        RLE(mtf, rle);
        for (size_t i = 0; i < rle.size(); i++) {
            cout << rle[i].first << " " << rle[i].second << endl; 
        }

    } else if (cmd == "decompress") {

        vector<pair<int, int> > codes;
        pair<int, int> pii;
        while(cin >> pii.first >> pii.second) {
            codes.push_back(pii);
        }        
        
        vector<int> revRLE;
        ReverseRLE(codes, revRLE);

        vector<char> revMTF;
        ReverseMTF(revRLE, revMTF, alf);

        string outputText;
        ReverseBWT(revMTF, outputText, alf);
        cout << outputText << endl;

    } else {
        cout << -1;
    }

    return 0;
}