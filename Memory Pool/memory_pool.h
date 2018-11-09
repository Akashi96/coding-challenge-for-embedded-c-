#ifndef MEMORY_POOL_H_INCLUDED
#define MEMORY_POOL_H_INCLUDED
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


#include <stdlib.h>
#include <stdbool.h>   // NOTE: c99 bool requires #include <stdbool.h>
#include <cstddef>


template <typename Type>
class freelist
{
public:
  std::stack<Type*> list;
  freelist(void* Head, size_t count, size_t block_size)
  {
    for(size_t i = 0 ; i < count; i++)
    { 
      // std::cout << (Type*)Head + block_size * i << std::endl;
      list.push((Type*)Head + block_size * i);
    }

  }
};

template <typename Type>
class blockElement
{
  Type value;
public:
  blockElement(freelist<Type>& free)
  {
    free.list.pop();
    std::cout << "Enter the value for Block-Element";
    std::cin >> value;
  }
  bool memory_pool_release(blockElement<Type>* obj, freelist<Type>& free)
  {
    free.list.push(*obj);
    // obj.~blockElement;
  }
};

void * memory_pool_init(size_t count, size_t block_size)
{
  return (void*) malloc(count* block_size);
}

// template <typename Type>
// bool memory_pool_destroy(void** mp, freelist<Type>& list)
// {
//   free(*mp);
//   *mp = NULL;
//   list.~freelist();
// }




// void * memory_pool_acquire(memory_pool_t *mp);

// convenience functions
// template <typename Type>
// size_t memory_pool_available(freelist<Type>& free)
// {
//   return free.list.size();
// }
// void memory_pool_dump(memory_pool_t *mp);

#endif // MEMORY_POOL_H_INCLUDED
