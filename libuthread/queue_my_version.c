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

// Since data is void**, you can pass oldest element which is void*
// by dereferencing data
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
	// void ** is an "output argument" meaning the function changes it
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
	void **cur_item = &queue->head;
	while (*cur_item != NULL) {
		if (*cur_item == data) {
			/*
				- similar approach to enqueue and dequeue functions
				- uses void ** to traverse the queue and update the pointers to remove the item
			*/
			// remove item from queue
			*cur_item = *((void **) *cur_item);
			return 0;
		}
		cur_item = (void **) *cur_item;
	}
	// Data not found in queue
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
	if (queue == NULL || func == NULL) {
		return -1;
	}

	// Create a new queue to store items to delete
	queue_t delete_q = queue_create();

	// Iterate through items in queue from oldest to newest item
	void **cur_item = &queue->head;
	while (*cur_item != NULL) {
		// call the callback function on current item
		void *data = *cur_item;
		void *next_item = *((void **) data);
		func(queue, data);
		if (*cur_item == data) {
			cur_item = (void **) next_item;
		} else {
			// callback function requested to delete the item
			// add item to the delete queue
			queue_enqueue(delete_q, data);
		}
	}
	// iterate through delete queue and delete each item
	void *data_to_delete;
	while (queue_dequeue(delete_q, &data_to_delete) == 0) {
		queue_delete(queue, data_to_delete);
	}

	// destroy the delete queue
	queue_destroy(delete_q);

	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL) {
		return -1;
	}
	return queue->size;
}
