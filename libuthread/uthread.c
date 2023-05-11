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
	// set running thread to next ready thread
	queue_dequeue(readyQueue, (void**) &runningThread);
	runningThread->state = RUNNING;

	// resume execution from context of running thread
	uthread_ctx_switch(previousThread->context, runningThread->context);
}

void uthread_yield(void)
{
	previousThread = runningThread;

	// move running thread back into ready queue
	queue_enqueue(readyQueue, previousThread);
	previousThread->state = READY;

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
	// allocate space for thread control block
	uthread_tcb* newThread = (uthread_tcb*) malloc(sizeof(uthread_tcb));
	if (newThread == NULL) {
		// memory allocation error
		return -1;
	}

	// allocate space for thread context
	uthread_ctx_t* context = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
	if (context == NULL) {
		// memory allocation error
		free(newThread);
		return -1;
	} else {
		newThread->context = context;
	}

	// allocate memory segment for stack
	void* stack = uthread_ctx_alloc_stack();
	if (stack == NULL) {
		// memory allocation error
		free(newThread);
		return -1;
	} else {
		newThread->stack = stack;
	}

	// initialize thread execution context
	int success = uthread_ctx_init(newThread->context, newThread->stack, func, arg);
	if (success == -1) {
		// context creation error
		uthread_ctx_destroy_stack(newThread->stack);
		free(newThread);
		return -1;
	}

	// add new thread to ready queue
	queue_enqueue(readyQueue, newThread);
	newThread->state = READY;
	
	return 0;
}

void uthread_destroy(uthread_tcb* thread) {
	// deallocate stack
	uthread_ctx_destroy_stack(thread->stack);
	// deallocate context
	free(thread->context);
	// deallocate thread
	free(thread);
}

static void uthread_remove(queue_t q, void *data) {
    uthread_tcb* thread = (uthread_tcb*) data;

	// clear thread
	uthread_destroy(thread);

	// remove from queue
	queue_delete(q, data);
}

void uthread_idle(void) {
	do  {
		// disable preemption?

		// yield to next thread
		uthread_yield();

		// enable preemption?
		
		// clear threads in exited queue
		queue_iterate(exitedQueue, uthread_remove);

	} while (queue_length(readyQueue) > 0); // while there are still ready threads in queue
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	int success;
	readyQueue = queue_create(); // queue for ready threads

	success = uthread_create(NULL, NULL); // add TCB for idle thread to queue (context overwritten on switch)
	if (success == -1) {
		// thread create error
		queue_destroy(readyQueue);
		return -1;
	}
	queue_dequeue(readyQueue, (void**) &runningThread); // set to running thread to facilitate context switch
	runningThread->state = RUNNING;
	
	success = uthread_create(func, arg); // add initial thread to queue
	if (success == -1) {
		// thread create error
		uthread_destroy(runningThread);
		queue_destroy(readyQueue);
		return -1;
	}
	exitedQueue = queue_create(); // queue for exited threads

	if (preempt) {

	}
	
	// begin thread execution
	uthread_idle();

	queue_destroy(readyQueue);
	queue_destroy(exitedQueue);

	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
	// set previous to thread (but don't put in any queue)
	// set status of active thread to blocked -- right order from previous line?
	// call context switch

	previousThread = runningThread;

	previousThread->state = BLOCKED;
	// in semaphore blocked queue, don't add to ready queue

	uthread_switch();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	// move unblocked thread back into ready queue
	queue_enqueue(readyQueue, uthread);
	uthread->state = READY;
}

