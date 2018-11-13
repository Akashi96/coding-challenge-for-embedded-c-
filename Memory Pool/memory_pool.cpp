// PRE_DEFINED LIBRARIES
#include <iostream>
#include <stack>
#include <typeinfo>
#include <stdlib.h>

// USER_DEFINED LIBRARIES
#include "memory_pool.h"

memory_pool_t * memory_pool_init(size_t count, size_t block_size)
{
	memory_pool_t* mempool;
	mempool = new memory_pool_t;
	mempool -> sizeof_pool = count * block_size;
	mempool -> block_available = count;
	mempool -> Head = (void*) malloc (mempool -> sizeof_pool);
	return mempool;
}

bool memory_pool_destroy(memory_pool_t *mp)
{
	free(mp->Head);	// Free the memory location pointed by Head, therefore, it frees the whole memory pool
	return 1;
}

void * memory_pool_acquire(memory_pool_t *mp)
{
	mp -> block_available -= 1;		// Reduce the number of blocks available by 1
	return mp -> free_ptr_at_top;	// Return the pointer to the availble block slot present at the top of stack and stored in variable memory_pool::free_ptr_at_top
}

bool memory_pool_release(memory_pool_t *mp, void * data)
{
	mp -> block_available += 1;		// Increase the number of blocks available by 1
	mp -> free_ptr_at_top = data;	// data is the pointer that points to the address that needs to be free. 
									// Simply, add this pointer to the freelist, so that it can be overwritten when required.
									// to do that, add it to the variable memory_pool::free_ptr_at_top. After returning, add this address to the freelist stack also.
	return 1;
}

size_t memory_pool_available(memory_pool_t *mp)
{
	return mp -> block_available;	// Return the value of variable memory_pool::block_available
}

int main()
{
	size_t count, block_size;
	std::cout << "Enter Number of blocks\n";
	std::cin >> count;
	std::cout << "Enter size of each block in bytes\n";
	std::cin >> block_size;
	
	// MEMORY POOL INIT
	memory_pool_t* mempool = memory_pool_init(count, block_size);	// Initialize memory Pool
	freelist<char>* free_list = new freelist<char>(mempool);		// create a freelist containing address of every available block slot
	mempool -> free_ptr_at_top = (void*) free_list -> list.top();

	int choice;
	void* ptr_to_free_space;

	while(1)
	{
	    std::cout<<"\n***********************MEMORY POOL****************************"
	        <<"\n Enter 1:To call memory_pool_destroy() --This function destroys the Memory Pool"
	        <<"\n Enter 2:To call memory_pool_acquire() -- This function returns a void* pointer to free memory location"
	        <<"\n Enter 3:To call memory_pool_release() -- Given a pointer to a memory location, thie function will release the data block allocated"
	        <<"\n Enter 4:To call memory_pool_available() -- Returns the number of blocks that can be inserted in the memory pool"
	        <<"\n Enter 5:To Exit"
	        <<"\n Enter your choice";
	    std::cin>>choice;
	    switch(choice)
	    {
	        case 1: std::cout<<"\n Memory Pool destroyed";
	                memory_pool_destroy(mempool);	// Call destroy function
	                delete mempool;		// delete dangling pointer
	                delete free_list;	// delete free_list
	                break;
	        
	        case 2: std::cout << "free address space at the top of freelist: " << mempool -> free_ptr_at_top << std::endl;
	        		ptr_to_free_space = memory_pool_acquire(mempool);
	        		free_list -> list.pop();	// remove the address allocated from the top of stack
	        		mempool -> free_ptr_at_top = (void*) free_list -> list.top();	// add the pointer at top of freelist to variable memory_pool::free_ptr_at_top
	        		std::cout << "top of list after calling memory_pool_acquire: " << mempool -> free_ptr_at_top << std::endl;
	        		std::cout << "Address returned by function: " << ptr_to_free_space << std::endl;;
	                break;
	       
	        case 3: memory_pool_release(mempool, ptr_to_free_space);
	        		std::cout << "Block at address " << ptr_to_free_space << " released from the Memory Pool" << std::endl;
	        		free_list -> list.push((char*) mempool -> free_ptr_at_top);		// add the pointer to newly released address at the top of free_list stack
	                break;
	        
	        case 4: std::cout << "There is space for allocating " << memory_pool_available(mempool) << " blocks in the memory pool" << std::endl;
	                break;

	        case 5: return 0;

	        default: std::cout<<"\n Wrong Choice";

	    }
	}
}