#include <iostream>
#include <algorithm>
#include "B-tree.hpp"

using std::pair;

bool elemCmp(Item i1, Item i2) {
    return i1.key < i2.key;
}

Btree::Btree() {
    this->root = nullptr;
}

Btree::~Btree() {
    RemoveBnode(this->root);
}

void Btree::RemoveBnode(Bnode *node) {
    if(node != nullptr) {
        for (size_t i = 0; i < node->keys.size(); i++) {
            if(!node->children[i]) {
                delete(node);
                break;
            }
            RemoveBnode(node->children[i]);
        }
    }
}

pair<Btree::Bnode*, int> Btree::FindIntern(Bnode *node, string k) {
    if(node == nullptr) {
        return {nullptr, -1};
    }
    int i = 0;
    for (; i < node->keys.size(); ++i) {
        if (node->keys[i].key == k) {
            return {node, i};
        }
        if (node->keys[i].key > k) {
            break;
        }
    }
    return FindIntern(node->children[i], k);
}

pair<uint64_t, Btree::ExitStatus> Btree::Find(string k) {
    pair<Bnode*, int> p = FindIntern(this->root, k);
    return {
            p.first->keys[p.second].value, 
            Btree::ExitStatus::OK
            };
}

// pair<Btree::Bnode*, Btree::ExitStatus> Btree::AddIntern(Item p, Bnode *node) {
    
// }

Btree::ExitStatus Btree::SplitRoot() {
    if(this->root == nullptr) {
        return Error;
    }

    if(root->keys.size() < 2 * (this->t) - 1) {
        return ExitStatus::DoesNotToBeSplitted;
    }

    Bnode *newNode = new Bnode;


    // root->keys.push_back(root->children[childIdx]->keys[this->t - 1]);
    // root->children.push_back(nullptr); // Tmp plug

    // for (int i = root->children.size() - 1; i > childIdx + 1; --i) {
    //     root->children[i] = root->children[i - 1]; 
    // }

    // root->children[childIdx + 1] = newNode;
    // newNode->isLeaf = root->children[childIdx]->isLeaf;
    // newNode->isRoot = root->children[childIdx]->isRoot;
    
    // // Распределим ключи разделенного ребенка
    // for (int i = t; i < 2 * t - 1; ++i) {
    //     newNode->keys.push_back(root->children[childIdx]->keys[i]);
    // }

    // // Модифицируем старый дочерний узел под индексом childIdx
    // for (int i = 2 * t - 2; i >= t - 1; --i) {
    //     root->children[childIdx]->keys.pop_back();
    // }

    // if(!root->children[childIdx]->isLeaf) {
    //     // Теперь нужно распределить детей разделенного ребенка
    //     for (int i = t; i < 2 * t; ++i) {
    //         newNode->children.push_back(root->children[childIdx]->children[i]);
    //     }

    //     for (int i = 2 * t - 1; i >= t; --i) {
    //         root->children[childIdx]->children.pop_back();
    //     }
    // }

    // // Сортируем вставкой(неважно, что за квадрат, так как N небольшое)
    // for (int i = 0; i < root->keys.size(); ++i) {
    //     Item x = root->keys[i];
    //     int j = 0;
    //     while(root->keys[j].key < x.key) {
    //         ++j;
    //     }
    //     for (int c = i; c > j; --c) {
    //         root->keys[c] = root->keys[c - 1];
    //     }
    //     root->keys[j] = x;
    // }
    
    // return ExitStatus::OK;

}


Btree::ExitStatus Btree::SplitChild(Btree::Bnode *node, int childIdx) {
    if(node == nullptr) {
        return ExitStatus::Error;
    }
    if(node->children[childIdx] == nullptr) {
        return ExitStatus::Error;
    }
    if(node->children[childIdx]->keys.size() < 2 * (this->t) - 1) {
        return ExitStatus::DoesNotToBeSplitted;
    }

    node->keys.push_back(node->children[childIdx]->keys[this->t - 1]);
    node->children.push_back(nullptr); // Tmp plug

    for (int i = node->children.size() - 1; i > childIdx + 1; --i) {
        node->children[i] = node->children[i - 1]; 
    }

    Bnode *newNode = new Bnode;
    node->children[childIdx + 1] = newNode;
    newNode->isLeaf = node->children[childIdx]->isLeaf;
    newNode->isRoot = node->children[childIdx]->isRoot;
    
    // Распределим ключи разделенного ребенка
    for (int i = t; i < 2 * t - 1; ++i) {
        newNode->keys.push_back(node->children[childIdx]->keys[i]);
    }

    // Модифицируем старый дочерний узел под индексом childIdx
    for (int i = 2 * t - 2; i >= t - 1; --i) {
        node->children[childIdx]->keys.pop_back();
    }

    if(!node->children[childIdx]->isLeaf) {
        // Теперь нужно распределить детей разделенного ребенка
        for (int i = t; i < 2 * t; ++i) {
            newNode->children.push_back(node->children[childIdx]->children[i]);
        }

        for (int i = 2 * t - 1; i >= t; --i) {
            node->children[childIdx]->children.pop_back();
        }
    }

    // Сортируем вставкой(неважно, что за квадрат, так как N небольшое)
    for (int i = 0; i < node->keys.size(); ++i) {
        Item x = node->keys[i];
        int j = 0;
        while(node->keys[j].key < x.key) {
            ++j;
        }
        for (int c = i; c > j; --c) {
            node->keys[c] = node->keys[c - 1];
        }
        node->keys[j] = x;
    }
    
    return ExitStatus::OK;    
}

pair<Btree::Bnode*, Btree::ExitStatus> Btree::Add(Item p) {




}

void Btree::ShowTreeIntern(Bnode *node) {
    if(node == nullptr) {
        return;
    }

    for (int i = 0; i < node->keys.size(); ++i) {
        std::cout << "{" <<  node->keys[i].key << ", " << node->keys[i].value << "}  ";
    }
    std::cout << std::endl;
    for (int i = 0; i < node->children.size(); ++i) {
        ShowTreeIntern(node->children[i]);
        std::cout << "   |   "; 
    }
    std::cout << std::endl;
    
}

void Btree::ShowTree() {
    ShowTreeIntern(this->root);
}

