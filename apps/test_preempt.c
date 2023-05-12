#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include <uthread.h>

/*
    Preempt tester
    - should continue to loop for each thread before yielding to the next
    - if preempt is set to false, then it will keep looping for one thread
*/

// Referenced Professor Porquet's example from uthread_yield.c

void thread0(void *arg)
{
	(void)arg;
	for (int i = 0; i < 50000; i++) {
		printf("Thread 0 Running\n");
	}
	printf("Thread 0 Ending\n\n");

	printf("Bye, ungrateful world...\n\n");
	raise(SIGKILL);
}

void thread3(void *arg)
{
	(void)arg;
	uthread_create(thread0, NULL);

	while (1) {
		printf("Thread 3 Running\n");
	}
}

void thread2(void *arg)
{
	(void)arg;
	uthread_create(thread3, NULL);

	while (1) {
		printf("Thread 2 Running\n");
	}
}

void thread1(void *arg)
{
	(void)arg;
	uthread_create(thread2, NULL);

	while (1) {
		printf("Thread 1 Running\n");
	}
}

int main(void)
{
    printf("Main thread started\n");
    // Set preempt to true
	uthread_run(true, thread1, NULL);

	return 0;
}
