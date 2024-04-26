#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

const int MAX_KEY_SIZE = 256;

struct Item {
    string key;
    uint64_t value;
    bool operator==(Item &i2) {
        return this->key == i2.key;
    }
    bool operator<(Item &i2) {
        return this->key < i2.key;
    }
    bool operator<=(Item &i2) {
        return this->key <= i2.key;
    }
    bool operator>=(Item &i2) {
        return this->key >= i2.key;
    }
    bool operator>(Item &i2) {
        return this->key > i2.key;
    }
};

class BTree {
private:
    int t_;
    struct BNode {
        int t, size;
        bool isNotLeaf;
        vector<Item> keys;
        vector<BNode*> children;
        vector<int> subTreeSize;

        BNode(int t, int size, bool isNotLeaf)
            : t(t), size(size), isNotLeaf(isNotLeaf) {
            keys.resize(2 * t - 1);
            children.resize(2 * t);
            subTreeSize.resize(2 * t);
        }

        int Place(string &val) {
            for (int i = 0; i < size; ++i) {
                if (val < keys[i].key) {
                    return i;
                }
            }
            return size;
        }

        void InsertKey(int indx, Item &val) {
            for (int i = size; indx < i; --i) {
                keys[i] = keys[i - 1];
            }
            keys[indx] = val;
            size += 1;
        }
        void InsertChild(int indx, BNode* node, int subtreesz) {
            for (int i = size; indx < i; --i) {
                children[i] = children[i - 1];
                subTreeSize[i] = subTreeSize[i - 1];
            }
            children[indx] = node;
            subTreeSize[indx] = subtreesz;
        }

        void SplitChild(int indx) {
            if (!isNotLeaf) {
                return;
            }
            BNode* child0 = children[indx];
            BNode* child1 = new BNode(t, t - 1, child0->isNotLeaf);
            int lsubtreesz = subTreeSize[indx] - t;
            int rsubtreesz = t - 1;
            for (int i = t; i < 2 * t - 1; ++i) {
                child1->keys[i - t] = child0->keys[i];
                child1->children[i - t] = child0->children[i];
                child1->subTreeSize[i - t] = child0->subTreeSize[i];
                rsubtreesz += child0->subTreeSize[i];
                lsubtreesz -= child0->subTreeSize[i];
            }
            child1->children[t - 1] = child0->children[2 * t - 1];
            child1->subTreeSize[t - 1] = child0->subTreeSize[2 * t - 1];
            rsubtreesz += child0->subTreeSize[2 * t - 1];
            lsubtreesz -= child0->subTreeSize[2 * t - 1];
            InsertKey(indx, child0->keys[t - 1]);
            InsertChild(indx + 1, child1, rsubtreesz);
            child0->size = t - 1;
            subTreeSize[indx] = lsubtreesz;
        }

        void MergeChildren(int indx) {
            if (!isNotLeaf) {
                return;
            }
            BNode* right = children[indx + 1];
            BNode* left = children[indx];
            left->keys[t - 1] = keys[indx];
            for (int i = 0; i < t - 1; ++i) {
                left->keys[t + i] = right->keys[i];
                left->children[t + i] = right->children[i];
                left->subTreeSize[t + i] = right->subTreeSize[i];
            }
            left->children[2 * t - 1] = right->children[t - 1];
            left->subTreeSize[2 * t - 1] = right->subTreeSize[t - 1];
            left->size = 2 * t - 1;
            subTreeSize[indx] += subTreeSize[indx + 1] + 1;
            DeleteKey(indx);
            DeleteChild(indx + 1);
            delete right;
        }

        void DeleteChild(int indx) {
            for (int i = indx; i <= size; ++i) {
                children[i] = children[i + 1];
                subTreeSize[i] = subTreeSize[i + 1];
            }
        }

        void DeleteKey(int indx) {
            for (int i = indx; i < size - 1; ++i) {
                keys[i] = keys[i + 1];
            }
            size -= 1;
        }

    };

    BNode* root_;

    pair<bool, Item> Find(Item &val, BNode* node) {
        int indx = node->Place(val.key);
        if (indx != 0 && val.key == node->keys[indx - 1].key) {
            val = node->keys[indx - 1];
            return {true, val};
        }
        if (node->isNotLeaf) {
            return (Find(val, node->children[indx]));
        }
        return {false, val};
    }
    

    void Insert(Item &val, BNode* node) {
        if (!node->isNotLeaf) {
            node->InsertKey(node->Place(val.key), val);
            return;
        }
        int indx = node->Place(val.key);
        if ((node->children[indx])->size == 2 * t_ - 1) {
            node->SplitChild(indx);
            if (val >= node->keys[indx]) {
                indx += 1;
            }
        }
        node->subTreeSize[indx] += 1;
        Insert(val, node->children[indx]);
    }

    int Procure(int indx, BNode* node) {
        BNode* son = node->children[indx];
        if (son->size >= t_) {
            return indx;
        }
        if (indx != 0 && (node->children[indx - 1])->size >= t_) {
            BNode* lbroth = node->children[indx - 1];
            int lsize = lbroth->size;
            int lsubtreesz = lbroth->subTreeSize[lsize];
            son->InsertKey(0, node->keys[indx - 1]);
            son->InsertChild(0, lbroth->children[lsize], lsubtreesz);
            node->keys[indx - 1] = lbroth->keys[lsize - 1];
            node->subTreeSize[indx] += 1 + lsubtreesz;
            node->subTreeSize[indx - 1] -= (1 + lsubtreesz);
            lbroth->size -= 1;
            return indx;
        }
        if (indx != node->size && (node->children[indx + 1])->size >= t_) {
            BNode* rbroth = node->children[indx + 1];
            int rsubtreesz = rbroth->subTreeSize[0];
            son->InsertKey(t_ - 1, node->keys[indx]);
            son->InsertChild(t_, rbroth->children[0], rsubtreesz);
            node->keys[indx] = rbroth->keys[0];
            node->subTreeSize[indx] += 1 + rsubtreesz;
            node->subTreeSize[indx + 1] -= (1 + rsubtreesz);
            rbroth->DeleteKey(0);
            rbroth->DeleteChild(0);
            return indx;
        }
        if (indx != 0) {
            node->MergeChildren(indx - 1);
            return indx - 1;
        }
        if (indx != node->size) {
            node->MergeChildren(indx);
        }
        return indx;
    }

    Item EraseMax(BNode* node) {
        int size = node->size;
        if (!node->isNotLeaf) {
            Item max = node->keys[size - 1];
            node->DeleteKey(size - 1);
            return max;
        }
        int indx = Procure(size, node);
        node->subTreeSize[indx] -= 1;
        return EraseMax(node->children[indx]);
    }

    Item EraseMin(BNode* node) {
        if (!node->isNotLeaf) {
            Item min = node->keys[0];
            node->DeleteKey(0);
            return min;
        }
        node->subTreeSize[0] -= 1;
        return EraseMin(node->children[Procure(0, node)]);
    }

    bool Erase(Item &val, BNode* node) {
        int indx = node->Place(val.key);
        if (indx != 0 && val == node->keys[indx - 1]) {
            indx -= 1;
            if (!node->isNotLeaf) {
                node->DeleteKey(indx);
                return true;
            }
            if ((node->children[indx])->size >= t_) {
                node->keys[indx] = EraseMax(node->children[indx]);
                node->subTreeSize[indx] -= 1;
                return true;
            }
            if ((node->children[indx + 1])->size >= t_) {
                node->keys[indx] = EraseMin(node->children[indx + 1]);
                node->subTreeSize[indx + 1] -= 1;
                return true;
            }
            node->subTreeSize[indx] -= 1;
            node->MergeChildren(indx);
            if (node->size == 0) {
                root_ = node->children[0];
                delete node;
                node = root_;
            }
            return Erase(val, node->children[indx]);
        }
        if (!node->isNotLeaf) {
            return false;
        }
        indx = Procure(indx, node);
        bool was = Erase(val, node->children[indx]);
        if (was) {
            node->subTreeSize[indx] -= 1;
        }
        if (node->size == 0) {
            root_ = node->children[0];
            delete node;
        }
        return was;
    }

    void Clear(BNode* node) {
        if (node == nullptr) {
            return;
        }
        for (int i = 0; i <= node->size; ++i) {
            Clear(node->children[i]);
        }
        delete node;
    }

    void PrintTree(int floor, BNode* node) {
        if (node == nullptr) {
            return;
        }
        cout << floor << '|';
        for (int i = 0; i < node->size; ++i) {
            cout << "{key: " << node->keys[i].key << ", value: " << node->keys[i].value << "}";
        }
        cout << '\n';
        for (int i = 0; i <= node->size; ++i) {
            PrintTree(floor + 1, node->children[i]);
        }
    }

bool SaveKeys(BNode *node, ofstream &os) {
        if(node == nullptr) {
            return true;
        }
        int keysCount = node->size;
        os.write(reinterpret_cast<const char*>((&keysCount)), sizeof(keysCount));   
        if(os.fail()) {
            throw "Can't write";
        }
        for (int i = 0; i < node->size; ++i) {   
        
            int sizeOfKey = node->keys[i].key.size();
            os.write(reinterpret_cast<const char*>((&sizeOfKey)), sizeof(sizeOfKey));
            if(os.fail()) {
                throw "Can't write";
            }
            os.write(node->keys[i].key.c_str(), sizeOfKey);
            if(os.fail()) {
                throw "Can't write";
            }
            uint64_t value = node->keys[i].value;
            os.write(reinterpret_cast<const char*>((&value)), sizeof(value));
            if(os.fail()) {
                throw "Can't write";
            }
        }
        os.write(reinterpret_cast<const char*>((&node->isNotLeaf)), sizeof(node->isNotLeaf));
        if(os.fail()) {
            throw "Can't write";
        }
        return node->isNotLeaf; 
    }

    void SaveNode(BNode *node, ofstream &os) {
        if(node == nullptr) {
            return;
        }
        if(SaveKeys(node, os)) {
            for (int i = 0; i < node->size + 1; ++i) {
                SaveNode(node->children[i], os);
            }
        }
    }

    void LoadKeys(BNode *newNode, ifstream &is) {
        int newKeysCount;
        is.read(reinterpret_cast<char*>((&newKeysCount)), sizeof(newKeysCount));
        if(is.fail()) {
            throw "Invalid read";
        }
        newNode->size = newKeysCount;
        int keySize;
        for (int i = 0; i < newKeysCount; ++i) {
            Item newItem;
            char buffer[MAX_KEY_SIZE];
            is.read(reinterpret_cast<char*>((&keySize)), sizeof(keySize));
            if(is.fail()) {
                throw "Invalid read";
            }
            is.read(buffer, keySize);
            if(is.fail()) {
                throw "Invalid read";
            }
            for (int j = 0; j < keySize; ++j) {
                newItem.key += buffer[j];
            }
            is.read(reinterpret_cast<char*>((&newItem.value)), sizeof(newItem.value)); 
            if(is.fail()) {
                throw "Invalid read";
            }
            newNode->keys[i] = newItem; 
        }
        bool isNotLeaf;
        is.read(reinterpret_cast<char*>((&isNotLeaf)), sizeof(isNotLeaf));
        if(is.fail()) {
            throw "Invalid read";
        }
        newNode->isNotLeaf = isNotLeaf;
    }

    void LoadNode(BNode *node, ifstream &is) {
        LoadKeys(node, is);
        if(!node->isNotLeaf) {
            return;
        }
        for (int i = 0; i < node->size + 1; ++i) {
            node->children[i] = new BNode(t_, t_ - 1, false);
        }
        for (int i = 0; i < node->size + 1; ++i) {
            LoadNode(node->children[i], is);
        }
    }


public:
    BTree(int t) {
        t_ = t;
        root_ = new BNode(t_, 0, false);
    }
    ~BTree() { 
        Clear(root_); 
    }

    void PubPrintTree() { 
        PrintTree(0, root_); 
    }

    void PubInsert(Item &val) {
        if (root_->size == 2 * t_ - 1) {
            BNode* node = new BNode(t_, 0, true);
            int subtreesz = 2 * t_ - 1;
            for (int i = 0; i <= 2 * t_ - 1; ++i) { 
                subtreesz += root_->subTreeSize[i];
            }
            node->children[0] = root_;
            node->subTreeSize[0] = subtreesz;
            root_ = node;
            root_->SplitChild(0);
        }
        Insert(val, root_);
    }

    pair<bool, Item> PubFind(Item &val) { 
        return Find(val, root_); 
    }

    void PubErase(Item &val) { 
        Erase(val, root_); 
    }

    void PubSave(ofstream &os) {
        SaveNode(this->root_, os);
    }

    void PubLoad(ifstream &is) {
        LoadNode(this->root_, is);
    }

    void Swap(BTree &tree) {
        BTree tmp(this->t_);
        tmp.root_ = this->root_;
        this->root_ = tree.root_;
        tree.root_ = tmp.root_;
        tmp.root_ = nullptr;
    }
};


void ToUpperBound(string &s) {
    for (size_t i = 0; i < s.size(); ++i) {
        if((s[i] >= 'a') && (s[i] <= 'z')) {
            s[i] -= ('a' - 'A');
        }
    }
}

int main() {

    // std::ios::sync_with_stdio(false);
    // cin.tie(0);

    const int t = 3;
    BTree tree(t);

    Item val;
    string com;

    while(cin >> com) {

        if (com == "+") {
            cin >> val.key >> val.value;
            ToUpperBound(val.key);
            if(tree.PubFind(val).first) {
                cout << "Exist" << endl;
            }
            else {
                tree.PubInsert(val);
                cout << "OK" << endl;
            }
        }
        else if (com == "-") {

            cin >> val.key;
            ToUpperBound(val.key);
            if(!tree.PubFind(val).first) {
                cout << "NoSuchWord" << endl;
            }
            else {
                tree.PubErase(val);
                cout << "OK" << endl;
            }
        }
        
        else if(com == "!") {
            string com2, pathToFile;
            cin >> com2 >> pathToFile;

            if(com2 == "Save") {
                ofstream os(pathToFile, std::ios::binary);

                try {
                    tree.PubSave(os); 
                }
                catch(...) {
                    cout << "ERROR: saving" << endl;
                    continue;
                }
                cout << "OK" << endl;
            }
            else if(com2 == "Load") {
                ifstream is(pathToFile, std::ios::binary); 
                BTree *newTree = new BTree(t);
                try {
                    newTree->PubLoad(is);    
                }
                catch(...) {
                    cout << "ERROR: loading" << endl;
                    continue;
                }
                tree.Swap(*newTree);
                cout << "OK" << endl;
                delete newTree;
            }
        }
        else if (com == "PrintTree") {
            tree.PubPrintTree();
        }
        else {

            val.key = com;
            ToUpperBound(val.key);
            std::pair<bool, Item> el = tree.PubFind(val);
            if (el.first) {
                cout << "OK: " << el.second.value << endl;
            }
            else {
                cout << "NoSuchWord" << endl;
            }
        }
        
    }
    return 0;
}