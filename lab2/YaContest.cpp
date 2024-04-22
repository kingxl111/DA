#include <iostream>
#include <string>
#include "vector.hpp"

using std::cin;
using std::cout;
using std::string;
using vector::Vector;

struct Item {
    string key;
    uint64_t value;
};

struct nullItem { //Класс, выполняющий функции optional. Возвращается публичными интерфейсами методом, ищущих upperbound и lowerbound и
    Item val;     //k-ую статистику для того, чтобы обработать ситуацию, когда запрашиваемой величину в дереве не существует
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
    struct Node { //реализация узла в виде структуры, приватных полей нет
        int t, size; //t копируется из дерева, в котором создан данный Node, size это количество ключей в узле
        bool inner; //флаг, хранящий информацию о том, является ли узел внутренним, т.е. не листовым, это видимо понадобится
        Vector<Item> keys; //вектор ключей
        Vector<Node*> sons; //вектор указателей на узлы-потомки данного узла
        Node(int t, int size, bool inner) //параметрический конструктор узла, написан плохо оттого, что я не знал конструкторы Vector
            : t(t), size(size), inner(inner) { //лучше вызвать конструкторы Vector, создающие их сразу нужного размера
            keys.resize(2 * t - 1); //во всех узлах векторы сразу максимально большого размера, чтобы можно было свободно обращаться
            sons.resize(2 * t); //смысла резервировать для них размер меньше думаю нет, так как при одном авторасширении вектор уже станет больше 2k - 1
        }

        void InsertKey(int indx, Item el) { //метод узла для вставки ключа val в позицию indx
            for (int i = size; indx < i; --i) { //цикл, сдвигающий вправо на 1 кусок массива [indx + 1, size - 1], чтобы затем вставить ключ в позицию i
                keys[i] = keys[i - 1];
            }
            keys[indx] = el; //вставка ключа
            size += 1; //обновление хранимого в узле размера
        }

        int Place(string val) { //метод, ищущий первый слева ключ, больший чем val (строгий upperbound)
            for (int i = 0; i < size; ++i) { //Замечание: если ключ val есть в Node, то вернёт (номер val) + 1
                if (val < keys[i].key) {
                    return i;
                }
            }
            return size; //если такого ключа нет, то возвращает size. Поскольку ключа с номером size нет, этот случай можно обработать отдельно.
        }

        void InsertSon(int indx, Node* node, int subtreesz) { //метод Node, вставляющий нового сына в массив ссылок на сыновей
            for (int i = size; indx < i; --i) { //вызывается только вместе с методом Insertkey
                sons[i] = sons[i - 1]; //сдвиг куска массива вправо для дальнейшей вставки как в Insertkey
            }
            sons[indx] = node;
        }

        void SplitSon(int indx) { //метод, который сына размера 2k - 1 под номером indx разделяет попалам на двух сыновей размера k - 1
            if (!inner) { // а серединный ключ вставляет в тот узел, от которого вызван метод
                return; //для листов ничего не делается
            }
            Node* child0 = sons[indx]; //разрубаемый сын становится новым левым сыном, потом его размер уменьшится
            Node* child1 = new Node(t, t - 1, child0->inner); //создается правый сын
            int rsubtreesz = t - 1;
            for (int i = t; i < 2 * t - 1; ++i) { //правый сын заполняется ключами, сыновьями и неявными ключами
                child1->keys[i - t] = child0->keys[i];
                child1->sons[i - t] = child0->sons[i];
            } //левого сына заполнять не надо, так как он ссылается на разрубаемого сына
            child1->sons[t - 1] = child0->sons[2 * t - 1]; //эти две строки почему не смог засунуть в цикл, видимо какой-то подводный есть
            InsertKey(indx, child0->keys[t - 1]); // в Node вставляется ключ, бывший в середине убиваемого сына
            InsertSon(indx + 1, child1, rsubtreesz); //вставляется созданный правый сын, разрубленный сын остаётся на месте и становится левым
            child0->size = t - 1; //размер левого сына уменьшается
        }

        void MergeSons(int indx) { // метод, сливющих детей размера t - 1 с номерами indx и indx + 1 в сына с номером indx размера 2t - 1
            if (!inner) {
                return; //для листа ниче не делается
            }
            Node* left = sons[indx];
            Node* right = sons[indx + 1];
            left->keys[t - 1] = keys[indx]; //в конец левого сына опускается ключ, разделающий детей с номерами indx и indx + 1
            for (int i = 0; i < t - 1; ++i) { //за опущенный сверху ключ в ключи левого сына копируются ключи, дети и неявные ключи правого
                left->keys[t + i] = right->keys[i];
                left->sons[t + i] = right->sons[i];
            }
            left->sons[2 * t - 1] = right->sons[t - 1];  //последнего сына правого сына и неявный ключ правого сына надо копировать в левого отдельно
            left->size = 2 * t - 1;
            DeleteKey(indx); //удаляется опущенный в слитого сына ключ
            DeleteSon(indx + 1); //в Node в массиве сыновей удаляется ссылка на сына с номером indx + 1, который был скопирован в сына с номером indx
            delete right; //right ссылается на сына с номером indx + 1 и очищает его
        }

        void DeleteKey(int indx) { //операция для корректного удаления ключа из Node, аналогична InsertKey
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
    }; //Вот и вся реализация Node, дальше реализация самого дерева

    Node* root_; //Поле для указателя на корневой узел

    bool Find(Item val, Node* node) { //Метод Find, ищет val в поддереве с корнем node и возвращает нашёл ли его
        int indx = node->Place(val.key); //ищет строгий upperbound в node и сохраняет его номер как indx
        if (indx != 0 && val.key == node->keys[indx - 1].key) { //если строгому upperbound предшествует сам val, то он есть в node и возвращается true
            return true; //также обработан случай, когда наибольший ключ не больше val, тогда Place вернёт node->size и val сравнится с последним ключом, который есть наибольший
        }
        if (node->inner) { //если строгому upperbound не предшествует val, то ему предшествует меньшее val число и val нет в узле, тогда спускаемся
            return (Find(val, node->sons[indx])); //в сына с номером indx, если node не лист
        }
        return false; //если val нет в node и node лист, то val нет в поддереве
    }

    nullItem StrictUpperBound(Item val, Node* node) { //ищет строгий upperbound в поддереве с корнем node
        int indx = node->Place(val.key); //также как в Find ищет строгий upperbound в node и сохраняет его номер в indx.
        if (indx == node->size) { // эта проверка срабатывает, когда val не меньше всех ключей в node
            if (node->inner) { //тогда, если node не лист, запускаемся от последнего его сына
                return StrictUpperBound(val, node->sons[indx]);
            }
            Item nl;
            return { nl, false }; //а в листе возвращает как раз nullItem в несуществующим значением, так как запрашиваемого элемента нема
        }
        if (node->inner) { //если верхняя проверка не сработала и node не лист, то strictupperbound либо равен keys[indx], либо лежит в sons[indx]
            nullItem ubound = StrictUpperBound(val, node->sons[indx]); //соответственно ищем его сначала в sons[indx]
            if (ubound.real) { //если он там нашёлся, то его возвращаем
                return ubound;
            }
            return { node->keys[indx], true }; //если его там нема, то strictupperbound это keys[indx]
        }
        return { node->keys[indx], true }; //в проверять sons[indx] не надо и сразу ответ keys[indx]
    }

    nullItem StrictLowerBound(Item val, Node* node) { //ищет наибольший меньший val ключ в поддереве с корнем node (strictlowerbound)
        if (val.key <= node->keys[0].key) { // если keys[0] это наименьший ключ в node, если val , keys[0], то либо strictlb в sons[0], либо его нет
            if (node->inner) { //в не листе ищем в sons[0]
                return StrictLowerBound(val, node->sons[0]);
            }
            Item nl;
            return { nl, false }; //в листе нет
        }
        int indx = node->Place(val.key); //если же в node есть ключи, меньшие val, то найдём strictub и запишем его номер в indx. Тогда val точно лежит между ключей с номерами indx - 1 и indx
        if (node->keys[indx - 1].key == val.key) { //если keys[indx] = val, то keys[indx - 1] уже точно наибольший ключ в node, меньший val
            indx -= 1;
        }
        if (node->inner) { //но если узел не лист, то стоит ещё поискать strictlb в сыне sons[indx]
            nullItem lbound = StrictLowerBound(val, node->sons[indx]);
            if (lbound.real) {
                return lbound;
            }
            return { node->keys[indx - 1], true }; //если там не нашёлся, то возвращаем keys[indx - 1]
        }
        return { node->keys[indx - 1], true }; //а листе сразу возвращаем keys[indx - 1]
    }

    void Insert(Item val, Node* node) { //вставляет ключ val в поддерево с корнем node
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

    int Procure(int indx, Node* node) { //эта хуйня ищет как в сына node с номером indx добавить один ключ, если его размер минимальный, т.е. t - 1
        Node* son = node->sons[indx]; //вставляться новый ключ будет в сына son
        if (son->size >= t_) { //если размер хотя бы t, то ничё не делает
            return indx;
        }
        if (indx != 0 && (node->sons[indx - 1])->size >= t_) { //если левый сосед существует и в нём не меньше t ключей, то заставит его поделиться
            Node* lbroth = node->sons[indx - 1];
            int lsize = lbroth->size;
            son->InsertKey(0, node->keys[indx - 1]); //резервирует место для ключа и нового сына в son
            // son->InsertSon(0, lbroth->sons[lsize], lsubtreesz);
            node->keys[indx - 1] = lbroth->keys[lsize - 1]; //копирует информацию из левого соседа
            lbroth->size -= 1; //удаляет информацию из левого соседа
            return indx;
        }
        if (indx != node->size && (node->sons[indx + 1])->size >= t_) { //заставляет делиться правого соседа, если он может, а левый нет
            Node* rbroth = node->sons[indx + 1];
            son->InsertKey(t_ - 1, node->keys[indx]); //также резервирует место в son
            // son->InsertSon(t_, rbroth->sons[0], rsubtreesz);
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
        return indx; //что то одно из вышеописанного видимо всегда должно сработать
    }

    Item EraseMax(Node* node) { //удаляет максимальный ключ из поддерева с корнем node и возвращает его
        int size = node->size;
        if (!node->inner) { // из листа просто удаление
            Item max = node->keys[size - 1];
            node->DeleteKey(size - 1);
            return max;
        }
        int indx = Procure(size, node); //если node не лист, то добавляет один ключ в последнего сына и запускается из него
        return EraseMax(node->sons[indx]);
    }

    Item EraseMin(Node* node) { //Удаляет минимальный ключ из поддерева с корнем node и возвращает его
        if (!node->inner) {
            Item min = node->keys[0];
            node->DeleteKey(0);
            return min;
        }
        return EraseMin(node->sons[Procure(0, node)]);
    }

    bool Erase(Item val, Node* node) { 
        int indx = node->Place(val.key); //ищет где предположительно может быть val в npde
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

    void Clear(Node* node) { //очистка дерева обходом postorder
        if (node == nullptr) {
            return;
        }
        for (int i = 0; i <= node->size; ++i) {
            Clear(node->sons[i]);
        }
        delete node;
    }

    void Traverse(int floor, Node* node) { //вывод содержимого узлов дерева с нулевого этажа до этажа floor обходом preorder, отсчёт этажей сверху вних
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
        root_ = new Node(t_, 0, false);
    }
    ~BTree() { Clear(root_); }

    void PubTraverse() { Traverse(0, root_); }

    void PubInsert(Item val) { //Отдельно надо обработать вставку, когда корень максимально допустимого размера
        if (root_->size == 2 * t_ - 1) {
            Node* node = new Node(t_, 0, true); //Создаётся новый корень, его единственным сыном становится старый корень, а ключом 0

            node->sons[0] = root_;
            root_ = node;
            root_->SplitSon(0); //корень разделяется
        }
        Insert(val, root_);
    }

    bool PubFind(Item val) { return Find(val, root_); }

    nullItem PubStrictUpperBound(Item val) { return StrictUpperBound(val, root_); }

    nullItem PubStrictLowerBound(Item val) { return StrictLowerBound(val, root_); }


    void PubErase(Item val) { Erase(val, root_); }
};



int main() {
    int t;
    char com;
    cin >> t;
    BTree tree(t);
    Item val;

    while(true) {
        cin >> com;
        switch (com) {
        case ('i'):
            cin >> val.key >> val.value;
            tree.PubInsert(val);
            break;
        case ('e'):
            cin >> val.key;
            tree.PubErase(val);
            break;
        case('f'):
            cin >> val.key;
            cout << tree.PubFind(val) << '\n';
            break;
        case('t'):
            tree.PubTraverse();
            break;
        case('u'):
            cin >> val.key;
            cout << tree.PubStrictUpperBound(val) << '\n';
            break;
        case('l'):
            cin >> val.key;
            cout << tree.PubStrictLowerBound(val) << '\n';
            break;
        default:
            break;
        }
    }
    return 0;
}