#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <uthread.h>

/*
    Preempt tester
    - should continue to loop for each thread before yielding to the next
    - if preempt is set to false, then it will keep looping for one thread
*/

// Referenced Professor Porquet's example from uthread_yield.c

void thread3(void *arg)
{
	(void)arg;
    printf("Thread 3 Begins\n");
	while (1) {
		printf("Thread 3 Running\n");
	}
    printf("Thread 3 Ends\n");

}

void thread2(void *arg)
{
	(void)arg;
    printf("Thread 2 Begins\n");
	while (1) {
		printf("Thread 2 Running\n");
	}
    printf("Thread 2 Ends\n");
}

void thread1(void *arg)
{
	(void)arg;
	uthread_create(thread2, NULL);
	uthread_create(thread3, NULL);

    printf("Thread 1 Begins\n");
	while (1) {
		printf("Thread 1 Running\n");
	}
    printf("Thread 1 Ends\n");
}

int main(void)
{
    printf("Main thread started\n");
    // Set preempt to true
	uthread_run(true, thread1, NULL);
    printf("Main thread completed\n");

	return 0;
}
