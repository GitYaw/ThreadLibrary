#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Looked at header file to find API documentation
#include "queue.h"
/* TODO Phase 1, for every function */

struct queue {
	// think of linked lists
	void *head;
	void *tail;
	int size;
};

queue_t queue_create(void)
{
	// Allocate memory for queue object
	queue_t new_q = (queue_t) malloc(sizeof(struct queue));
	
	// When allocating to new queue, check if fails
	if (new_q == NULL) {
		return NULL;
	}

	// Create queue
	new_q->head = NULL;
	new_q->tail = NULL;
	new_q->size = 0;

	return new_q;
}

int queue_destroy(queue_t queue)
{
	// Return -1 if queue is NULL or queue is not empty
	/*
		- queue->head is a way to check if queue is not empty
		- if head pointer is not NULL, then there should be a item in the queue
		- hence the queue is not empty
	*/
	if (queue == NULL || queue->head != NULL) {
		return -1;
	}

	// De-allocate memory
	free(queue);

	/*
		Another idea:
		- checking if queue is empty by creating a while loop
		- inside while loop, set a item to dequeue the queue and free it
		- outside while loop, free queue
	*/

	// Successfully destroyed queue
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	// For checking memory allocation error using queue and data
	if (queue == NULL || data == NULL) {
		return -1;
	}

	// Allocate memory for new queue item
	void **new_q = (void**) malloc(sizeof(void*));

	// Set it to the data and next pointer to NULL
	*new_q = data;
	// Essentially the next pointer
	*(new_q + 1) = NULL;

	// If empty, then update new_q
	if (queue->head == NULL) {
		queue->head = queue->tail = new_q;
	} else {
		// else, add to end of queue
		// cast void pointer to void** in order to increment the tail pointer by sizeof(void*)
		// essentially it is the tail pointing to next
		*((void**) queue->tail + 1) = new_q;
		queue->tail = new_q;
	}

	// Update queue size
	queue->size++;

	// De-allocate memeory for new_q
	// dereference it once to get a pointer to allocated memory block
	free(*new_q);

	// Successfully enqueued in queue
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	// For checking memory allocation error using queue and data
	// Check if queue is also empty
	if (queue == NULL || data == NULL || queue->head == NULL) {
		return -1;
	}

	// Dequeue oldest item in queue
	*data = queue->head;
	
	// Head pointer points to next item in queue
	queue->head = *((void**)queue->head);
	
	// Checks if queue is empty, helps avoid problems involving memory access
	// when there are no more items in queue
	if (queue->head == NULL) {
		queue->tail = NULL;
	}
	
	// Update queue size
	queue->size--;

	// Successful if data was set with oldest item available
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL || queue->head == NULL) {
		return -1;
	}

	// Find first (oldest) item in queue and delete
	

	// Data not found in queue
	return -1;

}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	if (queue == NULL) {
		return -1;
	}
	return queue->size;
}

