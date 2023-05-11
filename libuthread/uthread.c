#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

/// <-- remove triple-slash comments
enum State {RUNNING, READY, BLOCKED, EXITED};
typedef enum State state_t;

/*
 * uthread_tcb - Internal representation of threads called TCB (Thread Control
 * Block)
 */
struct uthread_tcb {
	state_t state;
	uthread_ctx_t* context;
	void* stack;
};
typedef struct uthread_tcb uthread_tcb;

queue_t readyQueue;
uthread_tcb* runningThread;
uthread_tcb* previousThread;
queue_t exitedQueue;

struct uthread_tcb *uthread_current(void)
{
	return runningThread;
}

void uthread_switch(void) {
	// Disable preempt because going to modify queue
	preempt_disable();

	// Set running thread to next ready thread
	queue_dequeue(readyQueue, (void**) &runningThread);
	runningThread->state = RUNNING;

	// Resume execution from context of running thread
	uthread_ctx_switch(previousThread->context, runningThread->context);

	// Enable preempt 
	preempt_enable();
}

void uthread_yield(void)
{
	// Store current thread into previousThread to remember it
	previousThread = runningThread;

	// Check if previous thread is running
	if (previousThread->state == RUNNING) {
		
		// Need to disable it because next step requires accessing global queue
		preempt_disable();

		// Move running thread back into ready queue
		queue_enqueue(readyQueue, previousThread);

		// Change it back to ready
		previousThread->state = READY;

		// Done with modifying global data structure
		preempt_enable();
	}

	uthread_switch();
}

void uthread_exit(void)
{
	previousThread = runningThread;

	// move running thread into exited queue (to be collected by idle thread)
	queue_enqueue(exitedQueue, previousThread);
	previousThread->state = EXITED;

	uthread_switch();
}

int uthread_create(uthread_func_t func, void *arg)
{
	// Allocate space for thread control block
	uthread_tcb* newThread = (uthread_tcb*) malloc(sizeof(uthread_tcb));
	if (newThread == NULL) {
		// Memory allocation error
		return -1;
	}

	/* Create thread */

	// Allocate space for thread context
	uthread_ctx_t* context = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
	if (context == NULL) {
		// Memory allocation error
		free(newThread);
		return -1;
	} else {
		newThread->context = context;
	}

	// Allocate memory segment for stack
	void* stack = uthread_ctx_alloc_stack();
	if (stack == NULL) {
		// Memory allocation error
		free(newThread);
		return -1;
	} else {
		newThread->stack = stack;
	}

	// Initialize thread execution context
	int success = uthread_ctx_init(newThread->context, newThread->stack, func, arg);
	if (success == -1) {
		// context creation error
		uthread_ctx_destroy_stack(newThread->stack);
		free(newThread);
		return -1;
	}

	newThread->state = READY;

	// Disable preempt before manipulating data structure queue
	preempt_disable();

	// Add new thread to ready queue
	queue_enqueue(readyQueue, newThread);

	// Done with modifying queue
	preempt_enable();
	
	return 0;
}

void uthread_destroy(uthread_tcb* thread) {
	// Deallocate stack
	uthread_ctx_destroy_stack(thread->stack);
	
	// Deallocate context
	free(thread->context);
	
	// Deallocate thread
	free(thread);
}

static void uthread_remove(queue_t q, void *data) {
    uthread_tcb* thread = (uthread_tcb*) data;

	// Clear thread
	uthread_destroy(thread);

	// Remove from queue
	queue_delete(q, data);
}

void uthread_idle(void) {
	do  {
		// Yield to next thread
		uthread_yield();
	
		// Clear threads in exited queue
		queue_iterate(exitedQueue, uthread_remove);

	} while (queue_length(readyQueue) > 0); // While there are still ready threads in queue
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// Should be called when uthread library is setting up preemption
	preempt_start(preempt);

	int success;

	// Accessing global queue, so remember to disable preempt
	preempt_disable();

	// Queue for ready threads
	readyQueue = queue_create(); 

	// Enable preempt after done with queue
	preempt_enable();

	// Failure to initalize the queue
	if (readyQueue == NULL) {
		return -1;
	}

 	// Add TCB for idle thread to queue (context overwritten on switch)
	success = uthread_create(NULL, NULL);
	
	if (success == -1) {
		// Thread create error
		queue_destroy(readyQueue);
		return -1;
	}
	// Set to running thread to facilitate context switch
	queue_dequeue(readyQueue, (void**) &runningThread); 
	runningThread->state = RUNNING;
	
	success = uthread_create(func, arg); // Add initial thread to queue
	if (success == -1) {
		// Thread create error
		uthread_destroy(runningThread);
		queue_destroy(readyQueue);
		return -1;
	}

	// Queue for exited threads
	exitedQueue = queue_create(); 

	// Begin thread execution
	uthread_idle();

	// Destroying queue 
	queue_destroy(readyQueue);
	queue_destroy(exitedQueue);

	// Call this function before uthread_run() returns
	// to get old signal alarm and timer 
	preempt_stop();

	return 0;
}

void uthread_block(void)
{
	// set previous to thread (but don't put in any queue)
	// set status of active thread to blocked -- right order from previous line?
	// call context switch

	previousThread = runningThread;
	previousThread->state = BLOCKED;
	// in semaphore blocked queue, don't add to ready queue

	// Part of yielding process
	uthread_switch(); 
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	// Check if thread is still blocked
	if (uthread->state == BLOCKED) {
		// Change state of thread to ready
		uthread->state = READY;

	}

	// Accessing global queue, so disable
	preempt_disable();

	// Move unblocked thread back into ready queue
	queue_enqueue(readyQueue, uthread);

	// Enable preempt after modifying queue
	preempt_enable();
}
