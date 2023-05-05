#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Looked at header file to find API documentation
#include "queue.h"

/*---------------- Node ---------------- */

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

/*---------------- Queue ---------------- */

struct queue {
	node_t front;
	node_t back;
	int size;
};

queue_t queue_create(void)
{
	// allocate memory for queue object
	queue_t queue = (queue_t)malloc(sizeof(struct queue));
	if (queue == NULL) {
		return NULL; // memory allocation error
	}

	// create queue
	queue->front = queue->back = NULL;
	queue->size = 0;
	return queue;
}

int queue_destroy(queue_t queue)
{
	// check if queue is NULL or queue is not empty
	if (queue == NULL || queue->size != 0) {
		return -1; // queue must be empty before deallocating
	}

	free(queue);

	return 0;
}

// remember first item is enqueued at the front and last item is at the back
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

	// checks if front of queue is empty
	if (queue->front == NULL) {
		// empty queue
		queue->front = queue->back = newNode;
	} else {
		// non-empty queue
		queue->back->next = newNode;
		// set back of queue to new node
		queue->back = newNode;
	}

	queue->size++;
	
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->size == 0) {
		return -1; // queue must contain node before dequeueing
	}
	
	node_t front = queue->front;
	
	// store node data in output, dequeue oldest node in queue
	*data = front->data;

	// set front of queue to next node
	queue->front = front->next;

	// delete front node
	node_destroy(front);

	queue->size--;

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL || queue->size == 0) {
		return -1;
	}

	// for keeping track of nodes
	node_t cur_node = queue->front;
	node_t prev_node = NULL;

	// find node to delete
	while (cur_node != NULL || queue->size != 0) {
		// checks if right node to delete
		if (cur_node->data == data) {
			if (prev_node == NULL) {
				// front node is the one to destroy
				queue->front = cur_node->next;
			} else {
				prev_node->next = cur_node->next;
			}

			// check if back node can be deleted
			if (cur_node == queue->back) {
				queue->back = prev_node;
			}

			queue_destroy(queue);
			node_destroy(cur_node);

			queue->size--;
			return 0;
		}

		// only executes when this node is empty
		if (prev_node == NULL) {
			prev_node = cur_node;
			cur_node = queue->front->next;
			continue;

		}

		prev_node = cur_node;
		cur_node = cur_node->next;
	}

	// node was not found
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue == NULL || func == NULL) {
			return -1;
	}

	int orig_size = queue->size;
	int cur_size = 0;

	node_t cur_node = queue->front;
	node_t prev_node = NULL;

	// iterate through nodes in queue from oldest to newest ones
	while (cur_node != NULL) {
		// call the callback function of current node
		func(queue, cur_node->data);
		cur_size = queue->size;
		// check to see if queue size was modified
		if (orig_size != cur_size) {
			// uses the previous node to continue where it left off
			// since the node that was deleted will no longer exist
			cur_node = prev_node->next;
			orig_size = cur_size;
			continue;
		}
		prev_node = cur_node;
		cur_node = cur_node->next;
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