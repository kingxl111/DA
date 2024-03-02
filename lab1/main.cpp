#include <iostream>
#include "sort.hpp"

int main() 
{
    
    TElem *ar = new TElem[MAX_ARRAY_SIZE];
    int arSize = 0;

    std::string key;
    std::string value;

    while(std::cin >> key >> value) 
    {
        ar[arSize].Set(key, value);
        ++arSize;
    }

    
    NSort::RadixSort(ar, arSize);

    for (int i = 0; i < arSize; ++i)
    {
        ar[i].PrintElem();
    }

    delete[] ar;

    return 0;
}