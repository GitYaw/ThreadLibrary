#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	int count;
	queue_t blockedQueue;
};

sem_t sem_create(size_t count)
{
	// Allocate space for semaphore
	sem_t semaphore = (sem_t) malloc(sizeof(struct semaphore));

	// Dealing with shared data structure, so disable preempt
	preempt_disable();

	// Create blocked queue for threads (waitlist)
	semaphore->blockedQueue = queue_create();

	// Enable preemp after done with queue
	preempt_enable();

	// Initialize count
	semaphore->count = count;

	return semaphore;
}

int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_length(sem->blockedQueue) != 0) {
		return -1;
	}

	queue_destroy(sem->blockedQueue);
	free(sem);	// Deallocate memory

	return 0;
}

int sem_down(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}

	// Check if resource is available
	if (sem->count > 0) {
		// Decrease number of available resources
		sem->count--;

	} else {
		struct uthread_tcb* thread = uthread_current();

		preempt_disable();

		// Add thread to waiting queue
		queue_enqueue(sem->blockedQueue, thread);

		preempt_enable();

		// Block thread
		uthread_block();
	}

	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}

	sem->count++;

	// Check if threads are waiting for resource
	if (queue_length(sem->blockedQueue) > 0) {

		// Acquire resource for next waiting thread
		sem_down(sem);

		// Unblock thread and remove from waiting queue
		struct uthread_tcb* thread;

		preempt_disable();

		queue_dequeue(sem->blockedQueue, (void**) &thread);

		preempt_enable();

		uthread_unblock(thread);
	}

	return 0;
}