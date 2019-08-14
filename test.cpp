#include <iostream>
#include <cilk/cilk.h>

#ifdef USE_CILK
#include <cilk/cilk_api.h>
#endif 

#include <pthread.h>


size_t NUM_ITERATIONS = 100000000;

// Don't optimize this fuction.
__attribute__((weak, noinline, optnone)) void loopFunction(size_t i, size_t* arr, pthread_t pid) {
	// Print the pthread and worker ID for the last iteration.
	if (i == NUM_ITERATIONS - 1) {
		std::cout << "Pthread ID for i = " << i << ":\t\t" << pthread_self() << "\n";

		#ifdef USE_CILK
		std::cout << "WorkerID for i = " << i << ":\t\t" <<__cilkrts_get_worker_number() << "\n";
		#endif
	}

	// Check that the pthread id in the loop is always the same as the one before the loop.
	if (!pthread_equal(pthread_self(), pid)) {
		std::cout << "NOT EQUAL (pid = " << pthread_self() << ")\n";
		exit(-1);
	}

	// Do something.
	arr[i] = i;
}

void* something(void* arg) {
	size_t* arr = new size_t[NUM_ITERATIONS];

	auto pthread_id = pthread_self();

	std::cout << "Pthread ID before loop:\t\t" << pthread_self() << "\n";

	#ifdef USE_CILK
	std::cout << "Worker ID before loop:\t\t" <<__cilkrts_get_worker_number() << "\n";
	#endif


//	#pragma cilk grainsize 1
	cilk_for ( size_t i = 0; i < NUM_ITERATIONS; ++i) {
		loopFunction(i, arr, pthread_id);
	}

	std::cout << "Pthread ID after loop:\t\t" << pthread_self() << "\n";

	#ifdef USE_CILK
	std::cout << "Worker ID after loop:\t\t" <<__cilkrts_get_worker_number() << "\n";
	#endif

	std::cout << arr[NUM_ITERATIONS - 1] << "\n";

	delete [] arr;

	return nullptr;
}

int fib(int n, pthread_t pid) {
	if (!pthread_equal(pthread_self(), pid)) {
		std::cout << "NOT EQUAL\n";
		exit(-1);
	}
	if (n < 2)
		return n;
	int x = cilk_spawn fib(n-1, pid);
	int y = fib(n-2, pid);
	cilk_sync;
	return x + y;
}

void* fibHandler(void* arg) {
	int n = *(int*)arg;
	fib(n, pthread_self());
	return nullptr;
}


int main(int argc, char** argv) {
	int numThreads = 1;
	if (argc > 1) {
		numThreads = atoi(argv[1]);
	}

	pthread_t* threads = new pthread_t[numThreads];

	int n = 20;

	for (size_t i = 0; i < numThreads; ++i) {

		pthread_create(threads + i, nullptr, &something, nullptr);

		// Uncomment for fib.
		//pthread_create(threads + i, nullptr, &fibHandler, &n);
	}

	for (size_t i = 0; i < numThreads; ++i) {
		pthread_join(threads[i], nullptr);
	}

	delete[] threads;
	
}
