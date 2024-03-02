#include <iostream>

const int KEY_LENGTH = 32;
const int VALUE_LENGTH = 64;
const int MAX_ARRAY_SIZE = 1000000;


class TElem 
{
private:
    int valueSize;
public:
    char key[KEY_LENGTH];
    char value[VALUE_LENGTH];
    void PrintElem();
    void Set(std::string &key, std::string &value);
};


void TElem::PrintElem() 
{
    for (int i = 0; i < KEY_LENGTH; ++i)
    {
        std::cout << this->key[i];
    } 
    std::cout << '\t';
    for (int i = 0; i < this->valueSize; ++i)
    {
        std::cout << this->value[i];
    }
    std::cout << '\n';
}

void TElem::Set(std::string &k, std::string &v) 
{
    for (size_t i = 0; i < KEY_LENGTH; ++i)
    {
        this->key[i] = k[i];
    }
    for (size_t i = 0; i < v.size(); ++i)
    {
        this->value[i] = v[i];
    }
    this->valueSize = v.size();
}

int HexadecimalToInt(char s)
{
    int ans;
    if((s >= '0') && (s <= '9'))
    {
        ans = s - '0';
    }
    else if((s >= 'a') && (s <= 'f')) 
    {
        ans = s - 'a' + 10;
    }
    else if((s >= 'A') && (s <= 'F')) 
    {
        ans = s - 'A' + 10;
    }
    return ans;
}

void RadixSort(TElem *ar, int arSize) 
{
    // 32 * 4 = 128 >> 64
    TElem *newAr = new TElem[arSize];

    for(int j = KEY_LENGTH - 1; j >= 0; --j)
    {
        // CountSort
        int count[16];
        for(int k = 0; k < 16; ++k) 
        {
            count[k] = 0;
        }

        for(int i = 0; i < arSize; ++i) 
        {
            int idx = HexadecimalToInt(ar[i].key[j]);
            ++count[idx];
        }

        int prev = 0;
        for(int k = 0; k < 16; ++k) 
        {
            count[k] += prev;
            prev = count[k];
        }

        for(int i = arSize - 1; i >= 0; --i) 
        {
            int idxInCount = HexadecimalToInt(ar[i].key[j]);
            int idxInResult = count[idxInCount] - 1;
            newAr[idxInResult] = ar[i];
            --count[idxInCount];
        }

        TElem *tmp = ar;
        ar = newAr;
        newAr = tmp;
    }

    delete[] newAr;

}

int main() 
{

    std::cin.tie(0);
    std::ios::sync_with_stdio(false);
    
    TElem *ar = new TElem[MAX_ARRAY_SIZE];

    int arSize = 0;

    std::string key;
    std::string value;

    while(std::cin >> key >> value) 
    {
        ar[arSize].Set(key, value);
        ++arSize;
    }

    
    RadixSort(ar, arSize);

    for (int i = 0; i < arSize; ++i)
    {
        ar[i].PrintElem();
    }

    delete[] ar;

    return 0;
}