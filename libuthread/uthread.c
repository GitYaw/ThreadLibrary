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

/// <--
enum State {RUNNING, READY, EXITED};
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
	/* TODO Phase 2 */
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// Allocate space for thread control block
	uthread_tcb* initialThread = (uthread_tcb*) malloc(sizeof(uthread_tcb));
	if (initialThread == NULL) {
		// memory allocation error
		return -1;
	}

	// Allocate memory segment for stack
	void* stack = uthread_ctx_alloc_stack();
	if (stack == NULL) {
		// memory allocation error
		free(initialThread);
		return -1;
	} else {
		initialThread->stack = stack;
	}

	// Initialize thread execution context
	if (uthread_ctx_init(initialThread->context, initialThread->stack, func, arg) == -1) {
		// context creation error
		uthread_ctx_destroy_stack(initialThread->stack);
		free(initialThread);
		return -1;
	} else {
		initialThread->state = READY;
	}

	/// enable preemptive scheduling if true

	readyQueue = queue_create();
	queue_enqueue(readyQueue, initialThread);

	// do {
	// 	// yield to next thread

	// 	/// if I understand this right, will continue here once it becomes active again
	// 	// deallocate stacks and threads for each thread in exited queue
	// 		/// use iterate & delete?

	// } while (/* size of ready queue > */ 0);

	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

