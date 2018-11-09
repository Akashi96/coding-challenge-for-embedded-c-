// PRE-DEFINED LIBRARIES
#include <iostream>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <condition_variable>

// USER-DEFINED LIBRARIES
#include "lock_guard.h"

// MACROS
#define numThreads 3

int thrSync(std::mutex& mutex, std::condition_variable& convar, bool& ready, int& my_turn, int i)
{		
		std::unique_lock<std::mutex> lock(mutex);	// std::unique_lock<>, the function convar.wait() only accepts unique_lock as an argument
													// because unlike lock_guard, the unique_lock<> can lock and unlock the mutex
		
		if(my_turn != i)	// Only the ith Thread will go to do its work, others will wait
			convar.wait(lock, [&i, &my_turn]{return my_turn == i;});	// the ith thread can only go ahead if the lock is unlocked and lambda function returns my_turn == i


		convar.wait(lock, [&ready]{return ready == true;});	// The thread can only perform work if ready == true
		std::cout << "Thread-" << i + 1 <<": Signal received, Doing Work..."<< std::endl;
		
		if(my_turn == numThreads -1)
			my_turn = 0;
		else
			my_turn++;
		std::this_thread::sleep_for(std::chrono::seconds(rand() % 5 + 1));	// Sleep for rand[1, 5]

		std::cout << "Thread-" << i + 1 <<": done with work, signal next thread"<< std::endl;
		lock.unlock();
		convar.notify_one();
		thrSync(mutex, convar, ready, my_turn, i);	// Recursively call the same function for continually doing the work. This enables the user to terminate the program using only ctrl+c 
}

int goInform(std::mutex& mutex, bool& ready)
{
	/* When the smart pointer std::unique_ptr<> points to a memory location, it prevents any other pointer to point to the same memory location, also the pointer cannot be copied or
	passed as a parameter. Also, std::make_unique<>() calls the constructor of the class to create an object and makes the unique_ptr<> to point to address of that object.
	Thus, unique_ptr<> becomes a good candidate to create an object of class lock_guard. Since this object can now never be shared, it follows the property of std::lock_guard which
	prevents the mutex to be used by other threads. Also, the lock_guard class that I defined will unlock the mutex only when the scope of the object of lock_guard claass gets over
	thus, mimicking the std::lock_guard function.*/
	std::unique_ptr<lock::lock_guard> ptr = std::make_unique<lock::lock_guard>(mutex);	// Use the user-defined lock_guard() to lock the mutex
	
	ready = true; // Make the variable ready = true, so that threads can start to do their work.
}

int main()
{
	std::thread threads[numThreads];	// array of threads
	std::mutex mutex;
	std::condition_variable convar;
	bool ready = false;
	int my_turn = 0;

	std::cout << "Main: Starting all Threads:\n";
	
	for(int i = 0; i < numThreads; i++)
	{
		threads[i] = std::thread(thrSync, std::ref(mutex), std::ref(convar), std::ref(ready), std::ref(my_turn), i);	// For each iteration, make a new thread and call function thrSync()
		std::cout << "Thread-" << i + 1 << ": Starting, Waiting\n";
		std::this_thread::sleep_for(std::chrono::microseconds(100));	// Delay the next iteration by 100 microseconds
	}
	
	// Executed by Main Thread 
	std::cout << "Starting Thread 1\n";
	goInform(mutex, ready);	// Make variable "ready" = true, so that other threads can pass the convar
	convar.notify_one();	// Inform just one thread about the change


	for(int i = 0; i < numThreads; i++)
	{
		threads[i].join();
	}

}