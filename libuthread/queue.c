#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"


/* Node */

typedef struct node* node_t;

/*
 * node_t - Node type
 *
 * A node is a data structure holding one data value and a link to another node.
 */
struct node {
    void* data;
	node_t next;
};

/**
 * Allocate a node with data.
 * 
 * Create a new object of type 'struct node' and return its address.
 *
 * @param data: Address of data item.
 * @return Pointer to new empty node. NULL in case of failure when allocating
 * the new node.
 */
node_t node_create(void* data) {
	node_t node = malloc(sizeof(struct node));
	if (node == NULL) {
		return NULL; // memory allocation error
	}

	node->data = data;
	node->next = NULL;
	return node;
}

/**
 * Deallocate a node.
 * 
 * Deallocate the memory associated to the node object pointed by `node`.
 *
 * @param node Node to deallocate.
 * @return -1 if `node` is NULL. 0 if `node` was successfully destroyed.
 */
int node_destroy(node_t node)
{
	if (node == NULL) {
		return -1;
	}

	free(node);

	return 0;
}

/* Queue */

struct queue {
	node_t front;
	node_t back;
	int size;
};

queue_t queue_create(void)
{
	queue_t queue = malloc(sizeof(struct queue));
	if (queue == NULL) {
		return NULL; // memory allocation error
	}

	queue->front = queue->back = NULL;
	queue->size = 0;
	return queue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->size != 0) {
		return -1; // queue must be empty before deallocating
	}

	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}

	node_t newNode = node_create(data);
	if (newNode == NULL) {
		// memory allocation error
		return -1;
	}

	// add new node to back of queue
	if (queue->back == NULL) {
		// empty queue
		queue->front = newNode;
	} else {
		// non-empty queue
		queue->back->next = newNode;
	}
	// set back of queue to new node
	queue->back = newNode;

	queue->size++;
	
	return 0;
}

// possible issue if back is not set to null if last item dequeued, especially if we enqueue after
int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->size == 0) {
		return -1; // queue must contain node before dequeueing
	}

	// Get front node and store data
	node_t frontNode = queue->front;
	*data = frontNode->data;

	// Set new front node
	queue->front = frontNode->next;
	free(frontNode);

	if (queue->front == NULL) {
		// empty queue, back just removed
		queue->back = NULL;
	}

	queue->size--;
	
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}
	
	// special case if first node (no previous node)
	if (queue->front->data == data) {
		queue_dequeue(queue, &data); // deleting first node is equivalent of dequeuing (data not used)
		return 0;
	}

	node_t node = queue->front;
	node_t back = queue->back;

	// in linked list, node must be deleted using the preceding node

	while (node != back) {
		// get next node in queue
		node_t nextNode = node->next;

		if (nextNode->data == data) {
			// node to delete, set node before to skip and point to node after
			node->next = nextNode->next;
			// deallocate node
			node_destroy(nextNode);

			queue->size--;
			return 0;
		}

		node = nextNode;
	}

	return -1; // data not found
}

int queue_iterate(queue_t queue, queue_func_t func)
{	 
	if (queue == NULL || func == NULL) {
		return -1;
	}

	node_t node = queue->front;
	while (node != NULL) {
		// queue must be delete-resistant, get next node before calling function
		node_t nextNode = node->next;
		// call callback function on data item
		func(queue, node->data);
		// proceed to next node
		node = nextNode;
	}
	
	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL) {
		return -1;
	}
	
	return queue->size;
}

