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
	sem_t semaphore = (sem_t) malloc(sizeof(semaphore));
	// Create blocked queue for threads (waitlist)
	semaphore->blockedQueue = queue_create();
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
	free(sem);

	return 0;
}

int sem_down(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}
	/* TODO Phase 3 */
	// if count > 0, decrement
	// else move uthread_current to waitlist
		// call block on uthread

	// check if resource is available
	if (sem->count > 0) {
		// decrease number of available resources
		sem->count--;
	} else {
		struct uthread_tcb* thread = uthread_current();

		// add thread to waiting queue
		queue_enqueue(sem->blockedQueue, thread);

		// block thread
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

	// check if threads are waiting for resource
	if (queue_length(sem->blockedQueue) > 0) {

		// acquire resource for next waiting thread
		sem_down(sem);

		// unblock thread and remove from waiting queue
		struct uthread_tcb* thread;
		queue_dequeue(sem->blockedQueue, (void**) &thread);
		uthread_unblock(thread);
	}

	return 0;
}

