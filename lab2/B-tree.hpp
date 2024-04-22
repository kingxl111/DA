#include <iostream>
#include <string>
#include "vector.hpp"

using vector::Vector;
using std::string;
using std::pair;

struct Item {
    string key;
    uint64_t value;
};

class Btree {
private:

    enum ExitStatus {
        OK, // Exist
        AlreadyExists, // OK : 34
        DoesNotExist, // NoSuchWord
        DoesNotToBeSplitted, // For SplitNode method
        Error
    };

    const int t = 8; // характеристическое для B-tree число t 

    struct Bnode {
        Vector<Item> keys;
        Vector<Bnode*> children;
        Bnode *parent;
        bool isLeaf;
        bool isRoot;
    };

    void RemoveBnode(Bnode *node);
    pair<Bnode*, int> FindIntern(Bnode *node, string k);
    // pair<Bnode*, ExitStatus> AddIntern(Item p, Bnode *node);
    ExitStatus SplitChild(Bnode* node, int childIdx);
    ExitStatus SplitRoot();
    void ShowTreeIntern(Bnode *node);


    Bnode* root;

public:
    Btree();
    ~Btree();

    pair<uint64_t, ExitStatus> Find(string k);
    pair<Bnode*, ExitStatus> Add(Item p);
    pair<Item, ExitStatus> Remove(string k);
    void ShowTree();

};