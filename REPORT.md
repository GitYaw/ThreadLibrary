# 

# Report on ECS150 Project 2: User-Level Thread Library

by Yaw Mireku and Jessica Trans

## Summary

This program implements a basic user-level thread library for Linux. Similar to
other existing thread APIs, the library can create new independent threads,
schedule the execution of threads in a round-robin fashion, provide an
interrupt-based scheduler, and make a thread-synchronization semaphore API. Its
implementation prevents both the convoy effect and starvation that could
otherwise occur as a result of multiple threads sharing the same processor and
resources.

## Implementation

The implementation of the program can be broken down into 4 phases, or stages.

1. Implementation of the queue API, which forms the data structure serving as
   the backbone for round-robin scheduling
2. Implementation of the user-level thread API, which defines the thread control
   block (TCB) and manages the context-switching between threads
3. Implementation of the semaphore API, which controls access to shared
   resources among multiple threads
4. Implementation of the preemption API, which uses timer interrupts to put
   constraints on processor time

### Makefile

The Makefile in the libuthread directory creates a static library archive called
libuthread/libuthread.a from the object files by including the rule: `ar -rcs $@
$^`.

### Phase 1 - queue API

Our queue uses a node data structure for its underlying implementation. Each
node stores a data value and a pointer to hold the address of the next node in
queue. These nodes come with a set of allocation/deallocation functions,
node_create() and node_destroy(). The queue itself stores the number of nodes in
the queue and pointers to the first and last node in the queue.

Our queue API contains standard FIFO queue functions. Most notably:

- queue_dequeue(): returns the oldest enqueued item and removes its node from
  the queue. Care was taken to ensure that if the removed node was the last in
  the queue, it was no longer stored as the back of the queue, which would cause
  errors when enqueuing again.
- queue_delete(): deletes the data item after finding the oldest item equal to
  the data. For this function, we handled the special case of deleting the first
  node in the queue by dequeuing the node and not using its value.
- queue_iterate(): goes through each item in the queue and calls the callback
  function on it. To make this iteration delete-resistant, the next node in the
  iteration is saved before calling the function.

### Queue Tester

- queue_tester.c
- queue_tester_example.c

The provided C program, queue_tester_example.c, tests if the queue is created
and whether or not the enqueue and dequeue functions worked. Using the
test_iteration() function example, it checks to see if the callback function
would increment every item in the queue. Another queue program tester was called
queue_tester.c which tested the API even more. There were test cases for edge
cases, iterate, delete, length, enqueue and dequeue, and create. These two queue
testers allowed us to check our queue API to see if it was functioning properly
in order to move on to the next phase. Knowing that the queue implementation is
resistant, this ensures that there will not be any errors when creating the
uthread API or moving on to the other phases.

### Phase 2 - uthread API

Since each thread has its own independent execution, the data structure for
uthread_tcb contains a state, context, and stack. The states keep track of
whether the thread is running, ready, blocked, or exited. The uthread API
functions by maintaining a queue for ready threads, a queue for exited threads,
and pointers to the current and previous threads executing. Threads are created
using uthread_create(), which allocates space for a new thread and moves it to
the back of the ready queue. Threads move from the ready queue to the current
thread, from which they proceed to the ready queue or exited queue based on
whether they yield or exit respectively.

The inspiration behind tracking the previous thread was to facilitate
context-switching. Since uthread_yield() and uthread_exit() (and later
uthread_block()) all involve context-switching to the next ready thread, a
separate function, uthread_switch(), was created to prevent code duplication.
This function sets the current thread to the next thread dequeued from the ready
queue and performs a context switch between the new current thread and the
previous one. For the very first context switch involving the main thread and
the initial thread, a "dummy" TCB is created and stored as the previous thread
so that the context of the main thread can be saved there when it yields
execution.

### Phase 3 - semaphore API

This API is made to be very flexible when it comes to managing resource sharing
and synchronization in concurrent programs. The semaphores are each implemented
with an internal counter representing the number of available resources and
their own queue for threads attempting to access a resource when none are
available. This blocked queue can be likened to a waitlist, as the first thread
in the queue is unblocked once a resource becomes available. Threads can acquire
or release a resource using sem_down() or sem_up() respectively, and the
internal account is increased/reduced accordingly.

To prevent a resource from becoming unavailable before an unblocked thread can
run again, which could potentially lead to starvation, we have the semaphore
call sem_down() again while unblocking the thread. This is because we recognize
that the count is just an internal indication of the number of active threads
that are using the resource. The unblocked queue joins the ready queue via
uthread_unblock() and is technically now another thread using the resource even
if it hasn't resumed execution. It was also worth noting that the unblocked
thread would continue after the sem_down() operation anyways and would therefore
require the semaphore to manipulate the count.

### Phase 4 - preemption

In order for this to be implemented properly, we modified both preempt.c and
uthread.c files. The preemption API prevents dangerous behavior such as never
calling the function uthread_yield() or blocking on a semaphore.

After referencing from the gnu libc source, the best way to create the
preempt_start() function was to install a signal handler that receives SIGVTALRM
and configure a timer that sends this alarm out every hundred times per second.
The two data structures used were itimerval for the timer and sigaction for the
signal handler. Once we confirmed that preempt is true, the program would create
a structure for the new action and set the alarm. ITIMER_VIRTUAL is an interval
timer that is associated with the process’s virtual CPU time, and not in
real-time. This value would decrement over time when the process is running in
user mode, meaning that it would send the SIGVTALRM signal when the timer
reaches zero.

The function preempt_stop() would use similar lines of code. It involves
restoring the previous timer as well as using SIG_DFL to terminate the process
after restoring to its default behavior. We incorporated another function that
serves as a timer interrupt handler, essentially calling the uthread_yield()
function after it checks for if the alarm variable matches SIGVTALRM.

The last two functions are the preempt_disable() and preempt_enable() functions.
They both call the same function sigprocmask() which modifies the signal mask by
blocking and unblocking signals. Whenever preempt is disable, then this function
would block the signal. On the other hand, in preempt_enable(), it unblocks the
signal.

In the uthread.c file, preempt_start() has to be called at the beginning of
uthread_run() in order to set up preemption for the uthread library. The same
applies to preempt_stop() which has to be called before uthread_run() returns.

Whenever the global data structure queue is accessed, it is a good practice to
disable preemption in order to prevent any errors or bugs using
preempt_disable(). For instance, when multiple threads are executed
concurrently, they can be preempted. Whenever a thread is preempting while
modifying a shared data structure like queue, it can lead to race conditions and
data corruption issues. After modifying the queue, then we can enable preempt
using preempt_enable().

### Preempt Tester

- test_preempt.c

We created a C program called test_preempt.c which essentially utilizes while
loops for each thread before it yields to the next thread due to the timer that
was implemented. It is important to set the preempt variable to true when
calling the uthread_run() function. If ./test_preempt.x was entered into the
terminal, it would demonstrate how the main thread begins thread1 and does not
yield to the CPU or any other threads until the timer is up before it continues
to the next thread and repeats the same process. In the preempt.c file, it
demonstrates how it creates a structure for a new action and will force the
currently running thread to yield for the next one once the timer expires.

### A Note on Memory Management

All objects that are allocated dynamically, such as the queues and TCBs, are
deallocated before going unused. Our memory management goes so far as to ensure
that even a data structure that fails partway through creation is deallocated
along with its data members.

## References

We used Joel Porquet-Lupine’s lecture and discussion slides as well as GNU libc
sources and static libraries. The provided resources in the instructions helped
with figuring out what to include in the programs. "Process Scheduling,"
"Concurrency Threads," and "Project 2" slides provided useful examples in
understanding how to implement the APIs and testers.