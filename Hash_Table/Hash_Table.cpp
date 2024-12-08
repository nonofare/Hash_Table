#include <iostream>
#include <string>
#include "HT.h"

int main()
{
    std::cout << "Hello World!\n";

    HT::HashTable<int>* ht = new HT::HashTable<int>();
    ht->Push("a", 10);
    std::cout << "Size: " << ht->Size() << " Capacity: " << ht->ArrayCapacity() << " Array Size: " << ht->ArraySize();

    return 0;
}