#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Item {
    string key;
    uint64_t value;
    bool operator==(Item i2) {
        return key == i2.key;
    }
    bool operator<(Item i2) {
        return key < i2.key;
    }
    bool operator<=(Item i2) {
        return key <= i2.key;
    }
    bool operator>=(Item i2) {
        return key >= i2.key;
    }
    bool operator>(Item i2) {
        return key > i2.key;
    }
};

struct nullItem {
    Item val;
    bool real;
};


class BTree {
private:
    int t_;
    struct Node {
        int t, size;
        bool isNotLeaf;
        vector<Item> keys;
        vector<Node*> children;
        vector<int> subTreeSize;

        Node(int t, int size, bool isNotLeaf)
            : t(t), size(size), isNotLeaf(isNotLeaf) {
            keys.resize(2 * t - 1);
            children.resize(2 * t);
            subTreeSize.resize(2 * t);
        }

        int Place(Item &val) {
            for (int i = 0; i < size; ++i) {
                if (val < keys[i]) {
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
        void InsertChild(int indx, Node* node, int subtreesz) {
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
            Node* child0 = children[indx];
            Node* child1 = new Node(t, t - 1, child0->isNotLeaf);
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
            Node* left = children[indx];
            Node* right = children[indx + 1];
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

    Node* root_;

    pair<bool, Item> Find(Item &val, Node* node) {
        int indx = node->Place(val);
        if (indx != 0 && val == node->keys[indx - 1]) {
            return {true, val};
        }
        if (node->isNotLeaf) {
            return (Find(val, node->children[indx]));
        }
        return {false, val};
    }

    void Insert(Item &val, Node* node) {
        if (!node->isNotLeaf) {
            node->InsertKey(node->Place(val), val);
            return;
        }
        int indx = node->Place(val);
        if ((node->children[indx])->size == 2 * t_ - 1) {
            node->SplitChild(indx);
            if (val >= node->keys[indx]) {
                indx += 1;
            }
        }
        node->subTreeSize[indx] += 1;
        Insert(val, node->children[indx]);
    }

    int Procure(int indx, Node* node) {
        Node* son = node->children[indx];
        if (son->size >= t_) {
            return indx;
        }
        if (indx != 0 && (node->children[indx - 1])->size >= t_) {
            Node* lbroth = node->children[indx - 1];
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
            Node* rbroth = node->children[indx + 1];
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

    Item EraseMax(Node* node) {
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

    Item EraseMin(Node* node) {
        if (!node->isNotLeaf) {
            Item min = node->keys[0];
            node->DeleteKey(0);
            return min;
        }
        node->subTreeSize[0] -= 1;
        return EraseMin(node->children[Procure(0, node)]);
    }

    bool Erase(Item &val, Node* node) {
        int indx = node->Place(val);
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

    void Clear(Node* node) {
        if (node == nullptr) {
            return;
        }
        for (int i = 0; i <= node->size; ++i) {
            Clear(node->children[i]);
        }
        delete node;
    }

    void PrintTree(int floor, Node* node) {
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

public:
    BTree(int t) {
        t_ = t;
        root_ = new Node(t_, 0, false);
    }
    ~BTree() { 
        Clear(root_); 
    }

    void PubPrintTree() { 
        PrintTree(0, root_); 
    }

    void PubInsert(Item &val) {
        if (root_->size == 2 * t_ - 1) {
            Node* node = new Node(t_, 0, true);
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
            // TODO: find the mistake... Why does it fall on 5?
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