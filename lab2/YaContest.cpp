#include <iostream>
#include <string>
#include "vector.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using vector::Vector;

struct Item {
    string key;
    uint64_t value;
};

struct nullItem { //Класс, выполняющий функции optional. Возвращается публичными интерфейсами методом, ищущих upperbound и lowerbound и
    Item val;     
    bool real; //хранит Item и bool. Если bool = false, то содержимого Item фактически не существует.
};

std::ostream& operator<<(std::ostream& os, const nullItem& nullval) { //оператор вывода в стандартный поток для класса nullItem
    if (nullval.real) {
        os << nullval.val.key << nullval.val.value;
        return os;
    }
    os << "none";
    return os;
}

class BTree {
private:
    int t_; //ранг дерева t, размер узлов меняется в диапазоне от t - 1 до 2t - 1
    struct BNode { 
        int t, size; 
        bool inner; //флаг, хранящий информацию о том, является ли узел внутренним, т.е. не является листом
        Vector<Item> keys; 
        Vector<BNode*> sons; 
        BNode(int t, int size, bool inner) : t(t), size(size), inner(inner) { 
            keys.resize(2 * t - 1); 
            sons.resize(2 * t); 
        }

        void InsertKey(int indx, Item &el) { // метод узла для простой вставки ключа val в позицию indx
            for (int i = size; indx < i; --i) { 
                keys[i] = keys[i - 1];
            }
            keys[indx] = el;
            size += 1; 
        }

        int Place(string &val) { //метод, ищущий первый слева ключ, больший чем val (строгий upperbound)
            for (int i = 0; i < size; ++i) { //Замечание: если ключ val есть в BNode, то вернёт (номер val) + 1
                if (val < keys[i].key) {
                    return i;
                }
            }
            return size; //если такого ключа нет, то возвращает size. Поскольку ключа с номером size нет
        }

        void InsertSon(int indx, BNode *node, int subtreesz) { //метод BNode, вставляющий нового сына в массив ссылок на сыновей
            for (int i = size; indx < i; --i) { //вызывается только вместе с методом Insertkey
                sons[i] = sons[i - 1]; 
            }
            sons[indx] = node;
        }

        void SplitSon(int indx) { //метод, который сына размера 2t - 1 под номером indx разделяет пополам на двух сыновей размера t - 1
            if (!inner) { // серединный ключ вставляет в тот узел, от которого вызван метод
                return; 
            }
            BNode* child0 = sons[indx]; //разделяемый сын становится новым левым сыном, потом его размер уменьшится
            BNode* child1 = new BNode(t, t - 1, child0->inner); //создается правый сын
            int rsubtreesz = t - 1;
            for (int i = t; i < 2 * t - 1; ++i) { //правый сын заполняется ключами и сыновьями 
                child1->keys[i - t] = child0->keys[i];
                child1->sons[i - t] = child0->sons[i];
            } 
            //левого сына заполнять не надо, так как он ссылается на разделяемого сына
            child1->sons[t - 1] = child0->sons[2 * t - 1]; 
            InsertKey(indx, child0->keys[t - 1]); 
            InsertSon(indx + 1, child1, rsubtreesz); //вставляется созданный правый сын
            child0->size = t - 1; //размер левого сына уменьшается
        }

        void MergeSons(int indx) { // метод, объединяющий детей размера t - 1 с номерами indx и indx + 1 в сына с номером indx размера 2t - 1
            if (!inner) { // проверка на то, что это лист
                return; 
            }
            BNode* left = sons[indx];
            BNode* right = sons[indx + 1];
            left->keys[t - 1] = keys[indx]; //в конец левого сына опускается ключ, разделающий детей с номерами indx и indx + 1
            for (int i = 0; i < t - 1; ++i) { //за опущенный сверху ключ в ключи левого сына копируются ключи и дети правого
                left->keys[t + i] = right->keys[i];
                left->sons[t + i] = right->sons[i];
            }
            left->sons[2 * t - 1] = right->sons[t - 1];  //последнего сына правого сына и неявный ключ правого сына надо копировать в левого отдельно
            left->size = 2 * t - 1;
            DeleteKey(indx); 
            DeleteSon(indx + 1); //в BNode в массиве сыновей удаляется ссылка на сына с номером indx + 1, который был скопирован в сына с номером indx
            delete right; //right ссылается на сына с номером indx + 1 и очищает его
        }

        void DeleteKey(int indx) { //операция для корректного удаления ключа из BNode, аналогична InsertKey
            for (int i = indx; i < size - 1; ++i) {
                keys[i] = keys[i + 1];
            }
            size -= 1;
        }

        void DeleteSon(int indx) { //аналогична InsertSon
            for (int i = indx; i <= size; ++i) {
                sons[i] = sons[i + 1];
            }
        }
    }; 

    BNode* root_;

    std::pair<bool, Item> Find(Item val, BNode* node) { //Метод Find, ищет val в поддереве с корнем node и возвращает нашёл ли его
        int indx = node->Place(val.key); //ищет строгий upperbound в node и сохраняет его номер как indx
        if (indx != 0 && val.key == node->keys[indx - 1].key) { //если строгому upperbound предшествует сам val, то он есть в node и возвращается true
            return {true, node->keys[indx - 1]}; //также обработан случай, когда наибольший ключ не больше val, тогда Place вернёт node->size и val сравнится с последним ключом, который есть наибольший
        }
        if (node->inner) { //если строгому upperbound не предшествует val, то ему предшествует меньшее val число и val нет в узле, тогда спускаемся
            return (Find(val, node->sons[indx])); //в сына с номером indx, если node не лист
        }
        Item plug;
        return {false, plug}; //если val нет в node и node лист, то val нет в поддереве
    }


    void Insert(Item val, BNode* node) { //вставляет ключ val в поддерево с корнем node
        if (!node->inner) { //для листа место для вставки ищется с помощью Place, вставка с помощью InsertKey
            node->InsertKey(node->Place(val.key), val);
            return;
        }
        int indx = node->Place(val.key); // для не листа также ищется два ключа, между которыми зажат val и вставка делается в сына между ними
        if ((node->sons[indx])->size == 2 * t_ - 1) { //если сын максимально возможного размера, то он разделяется на два
            node->SplitSon(indx);
            if (val.key >= node->keys[indx].key) {
                indx += 1;
            }
        }
        Insert(val, node->sons[indx]);
    }

    int Procure(int indx, BNode* node) { //функция ищет как в сына node с номером indx добавить один ключ, если его размер минимальный, т.е. t - 1
        BNode* son = node->sons[indx]; 
        if (son->size >= t_) { 
            return indx;
        }
        if (indx != 0 && (node->sons[indx - 1])->size >= t_) { //если левый сосед существует и в нём не меньше t ключей, то заставит его поделиться
            BNode* lbroth = node->sons[indx - 1];
            int lsize = lbroth->size;
            son->InsertKey(0, node->keys[indx - 1]); //резервирует место для ключа и нового сына в son

            node->keys[indx - 1] = lbroth->keys[lsize - 1]; //копирует информацию из левого соседа
            lbroth->size -= 1; //удаляет информацию из левого соседа
            return indx;
        }
        if (indx != node->size && (node->sons[indx + 1])->size >= t_) { //заставляет делиться правого соседа, если он может, а левый нет
            BNode* rbroth = node->sons[indx + 1];
            son->InsertKey(t_ - 1, node->keys[indx]); //также резервирует место в son

            node->keys[indx] = rbroth->keys[0];
            rbroth->DeleteKey(0); //удаляет информацию из правого соседа. Так как она в нём в начале, а не в конце, просто уменьшить размер нельзя
            rbroth->DeleteSon(0);
            return indx;
        }
        if (indx != 0) { //если с обоими соседями не вышло, но левый существует, то сливает с ним и возвращает индекс нового сына
            node->MergeSons(indx - 1);
            return indx - 1;
        }
        if (indx != node->size) { //если с левым не вышло, то аналогично с правым
            node->MergeSons(indx);
        }
        return indx; 
    }

    Item EraseMax(BNode* node) { //удаляет максимальный ключ из поддерева с корнем node и возвращает его
        int size = node->size;
        if (!node->inner) { // из листа просто удаление
            Item max = node->keys[size - 1];
            node->DeleteKey(size - 1);
            return max;
        }
        int indx = Procure(size, node); //если node не лист, то добавляет один ключ в последнего сына и запускается из него
        return EraseMax(node->sons[indx]);
    }

    Item EraseMin(BNode* node) { //Удаляет минимальный ключ из поддерева с корнем node и возвращает его
        if (!node->inner) {
            Item min = node->keys[0];
            node->DeleteKey(0);
            return min;
        }
        return EraseMin(node->sons[Procure(0, node)]);
    }

    bool Erase(Item val, BNode* node) { 
        int indx = node->Place(val.key); //ищет где предположительно может быть val в nоde
        if (indx != 0 && val.key == node->keys[indx - 1].key) { //если он нашёлся в node, то происходит удаление из node
            indx -= 1;
            if (!node->inner) { //если лист, то просто DeleteKey
                node->DeleteKey(indx);
                return true;
            }
            if ((node->sons[indx])->size >= t_) { // если в ближайщем левом сыне есть лишний ключ, то удаляет ключ макисмальный ключ из него и заменяет им удаляемый в node ключ 
                node->keys[indx] = EraseMax(node->sons[indx]);
                return true;
            }
            if ((node->sons[indx + 1])->size >= t_) { //аналогично с правым ближайщим сыном
                node->keys[indx] = EraseMin(node->sons[indx + 1]);
                return true;
            }
            node->MergeSons(indx);
            if (node->size == 0) { //если node корень, то он в результате этих действий мог лишиться ключей и остаться с одним сыном
              //так как размер корня не ограничен снизу t - 1
                root_ = node->sons[0]; //тогда новым корнем становится его единственный сын, а старый корень удаляется
                delete node;
                node = root_;
            }
            return Erase(val, node->sons[indx]); //если было слияние, то запускаемся от нового слитого сына
        }
        if (!node->inner) { //если node лист и в нём нет ключа, то ничё не происходит
            return false;
        }
        indx = Procure(indx, node); //если ключа нет в node, то в sons[indx] добавляется ключ, если его размер t - 1
        bool was = Erase(val, node->sons[indx]); //пытаемся удалить из sons[indx]
        if (node->size == 0) { //если node корень и в результате добавления ключа в sons[indx] node опустел, то обновляем корень
            root_ = node->sons[0];
            delete node;
        }
        return was;
    }

    void Clear(BNode* node) { //очистка дерева обходом postorder
        if (node == nullptr) {
            return;
        }
        for (int i = 0; i <= node->size; ++i) {
            Clear(node->sons[i]);
        }
        delete node;
    }

    void Traverse(int floor, BNode* node) { //вывод содержимого узлов дерева с нулевого этажа до этажа floor обходом preorder, отсчёт этажей сверху вних
        if (node == nullptr) {
            return;
        }
        cout << floor << '|'; //до палки выводится этаж, затем ключи через пробел
        for (int i = 0; i < node->size; ++i) {
            cout << "{key: " << node->keys[i].key << ", value: " << node->keys[i].value << "}";
        }
        cout << '\n';
        for (int i = 0; i <= node->size; ++i) {
            Traverse(floor + 1, node->sons[i]);
        }
    }

public:
    //публичные обёртки для методов
    BTree(int t) {
        t_ = t;
        root_ = new BNode(t_, 0, false);
    }
    ~BTree() { Clear(root_); }

    void PubTraverse() { Traverse(0, root_); }

    void PubInsert(Item val) { 
        //Отдельно надо обработать вставку, когда корень максимально допустимого размера
        if (root_->size == 2 * t_ - 1) {
            BNode* node = new BNode(t_, 0, true); //Создаётся новый корень, его единственным сыном становится старый корень, а ключом 0

            node->sons[0] = root_;
            root_ = node;
            root_->SplitSon(0); //корень разделяется
        }
        Insert(val, root_);
    }

    std::pair<bool, Item> PubFind(Item val) { 
        return Find(val, root_); 
    }
    
    void PubErase(Item val) { 
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

    int t = 3;
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
            // TODO: to create a load and save
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