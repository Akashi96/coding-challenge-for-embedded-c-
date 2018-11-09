// PRE_DEFINED LIBRARIES
#include <iostream>
#include <stack>
#include <stdlib.h>

// USER_DEFINED LIBRARIES
#include "memory_pool.h"

int main()
{
	// INITIATION OF MEMORY POOL
	size_t count, block_size;
	std::cout << "Enter the number of blocks allowed in the memory pool\n";
	std::cin >> count;
	std::cout << "Enter size of each block\n";
	std::cin >> block_size;
	void* Head = memory_pool_init(count, block_size);

	// CREATION OF FREE-LIST
	freelist<int> free(Head, count, block_size);

	// CREATION OF OBJECT TO BE PLACED IN THE MEMORY POOL
	blockElement<int> *obj = new(free.list.top()) blockElement<int>(free); // use of placement new to allocate the block at the address pointed by free.list.top() and call the constructor for blockElement

	// RELEASE ELEMENT FROM MEMORY POOL
	// obj->memory_pool_release(obj, free);

	// SIZE OF MEMORY POOL AVAILABLE
	// std::cout << "Memory Pool contains space for "<< memory_pool_available(free) << "Element Blocks\n";

	// DESTROY MEMORY POOL
	// memory_pool_destroy(&Head, free);
}