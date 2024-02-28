#include <iostream>
#include "sort.hpp"

void TElem::PrintElem() 
{
    for (int i = 0; i < KEY_LENGTH; ++i)
    {
        std::cout << this->key[i];
    } 
    std::cout << " ";
    for (int i = 0; i < this->valueSize; ++i)
    {
        std::cout << this->value[i];
    }
    std::cout << std::endl;
}

void TElem::Set(std::string k, std::string v) 
{
    for (size_t i = 0; i < k.size(); ++i)
    {
        this->key[i] = k[i];
    }
    for (size_t i = 0; i < v.size(); ++i)
    {
        this->value[i] = v[i];
    }
    if (v.size() < VALUE_LENGTH) 
    {
        for (int i = v.size(); i < VALUE_LENGTH; ++i)
        {
            this->value[i] = '\0';
        }
    }
    this->valueSize = v.size();
}

namespace NSort
{

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

void RadixSort(TElem ar[], int arSize) 
{
    // 32 * 4 = 128 >> 64
    TElem *newAr = new TElem[arSize];

    for(int j = KEY_LENGTH - 1; j >= 0; --j)
    {
        // CountSort
        int count[16];
        for(int k = 0; k < 16; ++k) {
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

        for(int i = 0; i < arSize; ++i) 
        {
            ar[i] = newAr[i];
        }
    }

    delete[] newAr;

}


} // namespace sort
