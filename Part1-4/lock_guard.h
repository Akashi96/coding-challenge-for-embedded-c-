#ifndef LOCK_GUARD_H_INCLUDED
#define LOCK_GUARD_H_INCLUDED

// INCLUDES
#include <memory>
#include <mutex>
#include <iostream>

namespace lock
{
    class lock_guard
    {  
    std::mutex* ptr;    // ptr will point to the address of mutex. This will help in unlocking the mutex when destructor will be called.
    public:
        lock_guard(std::mutex& mutex)
        {
            ptr = &mutex;
            mutex.lock();
        }
     
        ~lock_guard()
        {
            ptr->unlock(); // Unlock the mutex pointed by ptr
        }
    };
};

#endif // LOCK_GUARD_H_INCLUDED