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
	/* TODO Phase 3 */
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
}

