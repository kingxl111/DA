#include <iostream>
#include <chrono>
#include <algorithm>
#include "sort.hpp"

using duration_t = std::chrono::microseconds;
const std::string DURATION_PREFIX = "us";

/*
using duration_t = std::chrono::milliseconds;
const std::string DURATION_PREFIX = "ms";
*/

bool cmp(TElem el1, TElem el2)
{
    return el1.key < el2.key;
}

int main()
{
    TElem *input = new TElem[MAX_ARRAY_SIZE];
    TElem *input_stl = new TElem[MAX_ARRAY_SIZE];

    TElem elem;
    std::string key, value;
    int arSize = 0;
    while (std::cin >> key >> value)
    {
        input[arSize].Set(key, value);
        ++arSize;
    }

    for(int i = 0; i < arSize; ++i) 
    {
        input_stl[i] = input[i]; 
    }

    std::cout << "Count of lines is " << arSize << std::endl;

    // Измеряем время работы побитовой сортировки.
    std::chrono::time_point<std::chrono::system_clock> start_ts = std::chrono::system_clock::now();
    NSort::RadixSort(input, arSize);
    auto end_ts = std::chrono::system_clock::now();
    uint64_t RadixSort_ts = std::chrono::duration_cast<duration_t>( end_ts - start_ts ).count();

    // Измеряем время работы stl сортировки.
    start_ts = std::chrono::system_clock::now();
    std::stable_sort(input_stl, input_stl + arSize, cmp);
    end_ts = std::chrono::system_clock::now();

    uint64_t stl_sort_ts = std::chrono::duration_cast<duration_t>( end_ts - start_ts ).count();
    std::cout << "Radix sort time: " << RadixSort_ts << DURATION_PREFIX << std::endl;
    std::cout << "STL stable sort time: " << stl_sort_ts << DURATION_PREFIX << std::endl;

    delete[] input;
    delete[] input_stl;
}