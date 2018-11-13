/*
 * author: iancain
 * date: 10/1/2018
 * description:
 * Objective: speed operations of malloc/free and adapt idiomatically and separate memory
             management from other data storage patterns such as linked lists, stacks,
             double buffering
   Limitations: Fixed sized memory blocks. Due to the O(1) requirement only fixed sized
                memory allocation can be performed. Memory fragmentation and
                collection/collating operations are not desired due to performance demands

  Support O(1) operation in acquire and release operations
  Strategy:
    stack object to manage memory blocks
       acquire = pop_front  (acquire block off the front/top of stack)
       release = push_back  (release block by putting on back/bottom of stack)
 */

#include <stack>
#include <stdlib.h>
#include <stdbool.h>   // NOTE: c99 bool requires #include <stdbool.h>

typedef struct memory_pool
{
  void* Head, *free_ptr_at_top;   // Head points to the start of memory_pool; free_ptr_at_top points to the top of free_list stack
  int sizeof_pool;                // Contains the information about size of pool
  int block_available;            // Contains the information about number of block available to be allocated to the memory_pool
} memory_pool_t;

template <typename Type>
class freelist
{
  public:
  std::stack<Type*> list; // Define a stack to maintain freelist
  freelist(memory_pool_t* mempool)
  {
    for(size_t i = 0 ; i < mempool -> block_available; i++)
    { 
      std::cout << (void*)((Type*)mempool -> Head + sizeof(Type) * i) << std::endl;
      list.push((Type*)mempool -> Head + sizeof(Type) * i);  // It is assumed that the all the allocations in the memory_pool will be of same size. Therefore, if the Head pointer
                                                             // points to the start of pool, every new location should lie at block_size * i distance from it.
    }

  }
};


memory_pool_t * memory_pool_init(size_t count, size_t block_size);
bool memory_pool_destroy(memory_pool_t *mp);

void * memory_pool_acquire(memory_pool_t *mp);
bool memory_pool_release(memory_pool_t *mp, void * data);

// convenience functions
size_t memory_pool_available(memory_pool_t *mp);
  void memory_pool_dump(memory_pool_t *mp);