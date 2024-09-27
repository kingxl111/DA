#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


using namespace std;

/* Представим, что мы хотим разработать систему саджестов -
 * системы, которая поможет дополнять запросы пользователей.
 * На каждый введенный префикс система должна выдать наиболее
 * 3 близких к поисковому слову товары (расположены в лексиграфически.)
 *
 *
 * Ввод:
 * products = ["mobile","mouse","moneypot","monitor","mousepad"], searchWord = "mouse"
 * Вывод: [
 *          ["mobile","moneypot","monitor"], // для "m"
 *          ["mobile","moneypot","monitor"], // для "mo"
 *          ["mouse","mousepad"],            // для "mou"
 *          ["mouse","mousepad"],            // для "mous"
 *          ["mouse","mousepad"]             // для "mouse"
 *        ]
 */

class Trie {

private:
    class Node {
    public:
        char letter;
        vector<Node*> children;
        bool isLeaf;

        Node() {
            for (size_t i = 0; i < 26; ++i) {
                children.push_back(nullptr);
            }
            isLeaf = false;
        }

    };

public:
    Node* root;
    int leafCount;


    Trie() {
        leafCount = 0;
        root = new Node;
        root->isLeaf = false;
        root->children.resize(26); // Количество букв в английском алфавите
        
    }

    void Add(const string &st) {

        Node *trieNode = root;
        if(trieNode == nullptr) {
            return;
        }

        for (size_t i = 0; i < st.size(); ++i) {
            char patternLetter = st[i];
            if(trieNode->children[st[i] - 'a'] == nullptr) {
                Node *newNode = new Node();
                trieNode->children[st[i] - 'a'] = newNode;
                newNode->letter = st[i];
            }
            trieNode = trieNode->children[st[i] - 'a'];
        }

        trieNode->isLeaf = true;
    }

    void Find3Similar(vector<string> &answer, int &count, Node* curNode, string curString) {
        if((count == 3) || (curNode == nullptr)) {
            return;
        }

        // cout << curString << endl;
        if(curNode->isLeaf) {
            answer.push_back(curString);
            ++count;
        }

        for (size_t i = 0; i < curNode->children.size(); ++i) {
            if(curNode->children[i] != nullptr) {
                curString += (i + 'a');
                Find3Similar(answer, count, curNode->children[i], curString);
                curString.pop_back();
            }
        }
    }

    vector<vector<string>> suggestedProducts(vector<string>& products, string searchWord) {

        vector<vector<string> > ans;
        if(root == nullptr) {
            return ans;
        }

        // Добавим все строки в trie
        for (size_t i = 0; i < products.size(); ++i) {
            this->Add(products[i]);
        }
        
        string curWord = "";
        Node* prevNode = this->root;
        for (size_t i = 0; i < searchWord.size(); ++i) {
            curWord += searchWord[i];
            vector<string> curWords;
            if(prevNode->children[searchWord[i] - 'a'] != nullptr) {
                prevNode = prevNode->children[searchWord[i] - 'a'];
                
                int count = 0;
                Find3Similar(curWords, count, prevNode, curWord);
                // cout << endl;
            }
            ans.push_back(curWords);
        }
        return ans;
    }


    ~Trie() {}
};



int main() {

    vector<string> products = {"mobile","mouse","moneypot","monitor","mousepad"};
    string searchWord = "mouse";

    Trie trie;
    vector<vector<string> > ans = trie.suggestedProducts(products, searchWord);

    string pattern = "";
    for (size_t i = 0; i < ans.size(); ++i) {
        pattern += searchWord[i];
        cout << "pattern: " << pattern << endl << "Suggested products: ";
        for (size_t j = 0; j < ans[i].size(); ++j) {
           cout << ans[i][j] << " ";
        }
        cout << endl << endl;
    }

    return 0;
}