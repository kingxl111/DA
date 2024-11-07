#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {

    string s;
    cin >> s;
    size_t n = s.size();
    
    // mtx[i][j] - количество палиндромов в подстроке s[i:j]
    vector<vector<int64_t> > mtx(n);
    for (size_t i = 0; i < n; i++) {
        vector<int64_t> row(n, 0);
        mtx[i] = row;
    }

    // база индукции
    // в подстроке s[i:i] всего один палиндром - вся подстрока, состоящая из одного символа 
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j) {
                mtx[i][j] = 1;
            }
        }
    }

    // переход индукции
    for (size_t l = 1; l <= n; l++) {
        int i = 0;
        while(i + l < n) {
            // s[i] - первый символ в текущей подстроке, s[i+l] - последний 
            if(s[i] == s[i + l]) {
                // + 1, т.к. появился новый палиндром
                mtx[i][i + l] = mtx[i + 1][i + l] + mtx[i][i + l - 1] + 1;
            }
            else {
                // здесь чтобы два раза не учитывать одни и те же палиндромы, 
                // вычитаем количество палиндромов в подстроке без первого и последнего символов
                mtx[i][i + l] = mtx[i + 1][i + l] + mtx[i][i + l - 1] - mtx[i + 1][i + l - 1];
            }
            i++;
        }
    }
    
    cout << mtx[0][n - 1] << endl;

    return 0;
}
