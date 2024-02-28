#pragma once
#include <iostream>

const int KEY_LENGTH = 32;
const int VALUE_LENGTH = 64;
const int MAX_ARRAY_SIZE = 1e6;

class TElem 
{
private:
    int valueSize;
public:
    char key[KEY_LENGTH];
    char value[VALUE_LENGTH];
    void PrintElem();
    void Set(std::string key, std::string value);
};

namespace NSort
{

void RadixSort(TElem ar[], int size);

}