# Report on ECS150 Project 2: User-level Thread library
by Yaw Mireku and Jessica Trans

## Introduction to Project

The purpose of this project is to help us understand the concept of threads and implement a basic user-level thread library for Linux. This library would be capable, like any other existing libraries, of creating new independent threads, scheduling the execution of threads in a round-robin fashion, providing an interrupt-based scheduler, and making a thread synchronization semaphores API. 

This project consists of four phases: creating a queue, uthread, and semaphore API as well as implementing preemption. Our project consists of creating several C programs and Makefiles. We were given a skeleton code of the thread library containing two folders: apps and libuthread. The libuthread directory is where all the implementations for the library are created while the apps directory contains a couple of test applications to check if the C programs in the libuthread directory would work properly. 

## Makefile

Our Makefile in the libuthread directory was similar to the one provided in the apps directory. It will create a static library archive called libuthread/libuthread.a by including the rule “ar -rcs $@ $^”. This website, http://tldp.org/HOWTO/Program-Library-HOWTO/static-libraries.html, helped implement this.

## Phase 1 - queue API

Our project for phase 1 was to create a FIFO queue API. It contains various functions such as queue_create(), queue_destroy(), queue_enqueue(), queue_dequeue(), queue_delete(),
queue_iterate(), and queue_length(). We created two new functions called node_create() and node_destroy() for the data structure we included in order to store a node’s data value and link it to another node. 

In the queue data structure, it uses a linked list to implement it. The queue_iterate() function is unique because it takes in the queue and a function as its parameters which would be utilized to call the custom function for every data item in the queue. It is important to get the queue API to work properly because it can lead to an error when creating the uthread API.

### Queue Tester

The C program, queue_tester_example.c, was already provided and it tests if the queue is created and whether or not the enqueue and dequeue function worked. Using the test_iteration() function example, it checks to see if the callback function would increment every item in the queue. Another queue program tester was called queue_tester.c which tested the API even more. There were test cases for edge cases, iterate, delete, length, enqueue and dequeue, and create. These two queue testers allowed us to check our queue API to see if it was functioning properly in order to move on to the next phase. Knowing that the queue implementation is resistant, this ensures that there will not be any errors when creating the uthread API or moving on to the other phases.

## Phase 2 - uthread API

Since each thread has its own independent execution, the data structure for uthread_tcb would contain the state, context, and stack. The states keep track of whether the thread is running, ready, blocked, or exited. The context holds a backup of the CPU registers which it saves for later when restoring it. Lastly, every thread has its own stack. These threads run concurrently in the same address space of one process. The purpose of this uthread API is to enable a way to provide applications to use multithreading. This includes creating, running, terminating, or manipulating these threads in various ways.

### Uthread Tester

There are two testers for this API called uthread_hello.c and uthread_yield.c. The first tester is meant to check for the existence of one thread and check if it returns successfully. While the C program tester, uthread_yield.c, tests if the multiple threads are creating and yielding properly. This means that the parent thread should be returned by the child, and this can be shown in the terminal with the usage of print statements.

## Phase 3 - semaphore API

### Semaphore Tester

## Phase 4 - preemption



### Preempt Tester

This tester was not provided to us, so we had to create a C program called test_preempt.c which essentially utilizes while loops for each thread before it yields to the next thread due to the timer that was implemented. It is important to set the preempt variable to true when calling the uthread_run() function. If ./test_preempt.x was entered into the terminal, it would demonstrate how the main thread begins thread1 and does not yield to the CPU or any other threads until the timer is up before it continues to the next thread and repeats the same process. In the preempt.c file, it demonstrates how it creates a structure for a new action and will force the currently running thread to yield for the next one once the timer expires.

## Testing

We tested the queue API on our local machines while the other C programs required a Linux environment, meaning we would have to work on the CSIF computers. We would make sure to enter “make clean” then “make all” for both the apps and libuthread directories to ensure that our program is generating the proper executables. Once it has executed successfully, the next step was running the .x files in the apps directory. Unit testing was an efficient method of testing all the possible usage scenarios for our C programs in the libuthread directory.

## References
We used Joel Porquet-Lupine’s lecture and discussion slides as well as GNU libc sources. "Process Scheduling", "Concurrency Threads", and "Project 2" slides provided useful examples in understanding how to implement the APIs and testers.