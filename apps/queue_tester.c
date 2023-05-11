#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "queue.h"
queue_t q;

typedef void (*function_t)(); 

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

void test_create(void)
{
	TEST_ASSERT(q != NULL);
	queue_destroy(q);
	
	q = queue_create();
}

void test_enqueue() {
	int a, b;

	queue_enqueue(q, &a);
	queue_enqueue(q, &b);

	void* data;

	queue_dequeue(q, &data);
	TEST_ASSERT(data == &a);
	queue_dequeue(q, &data);
	TEST_ASSERT(data == &b);
}

void test_length() {
	int length;
	int a;
	for (int i = 0; i < 3; i++) {
		queue_enqueue(q, &a);
	}

	length = queue_length(q);
	TEST_ASSERT(length == 3);

	void* data;
	queue_dequeue(q, &data);

 	length = queue_length(q);
	TEST_ASSERT(length == 2);

	queue_dequeue(q, &data);
	queue_dequeue(q, &data);

	length = queue_length(q);
	TEST_ASSERT(length == 0);
}

void test_delete() {
	int a, b;
	queue_enqueue(q, &b);
	queue_enqueue(q, &a);
	queue_enqueue(q, &b); // q = &b, &a, &b

	queue_delete(q, &b); // q = &a, &b
	TEST_ASSERT(queue_length(q) == 2);
	void* data;
	queue_dequeue(q, &data);
	TEST_ASSERT(data == &a);
	queue_dequeue(q, &data);
	TEST_ASSERT(data == &b);

	int c;
	queue_enqueue(q, &a);
	queue_enqueue(q, &b);
	queue_enqueue(q, &c);  // q = &a, &b, &c

	queue_delete(q, &b); // q = &a, &c
	queue_delete(q, &c); // q = &a
	queue_delete(q, &a);
	TEST_ASSERT(queue_length(q) == 0);
}

static void increment(queue_t q, void *data) {
    int* i = (int*) data;
	if (*i >= 0) {
		(*i)++;
	} else {
		queue_delete(q, i);
	}
}

void test_iterate() {
	int ar[] = {0, 1, -2, -3};
	for (int i = 0; i < (int) (sizeof(ar) / sizeof(int)); i++) {
		queue_enqueue(q, ar + i);
	}

	queue_iterate(q, increment);
	TEST_ASSERT(ar[0] == 1);
	TEST_ASSERT(ar[1] == 2);
	int length = queue_length(q);
	TEST_ASSERT(length == 2);

	void* data;
	for (int i = 0; i < length; i++) {
		queue_dequeue(q, &data);
	}
}

void test_edge_cases() {
	fprintf(stderr, "*** TEST EDGE CASES ***\n");

	// queue is null
	q = NULL;
	void* data = &q;
	function_t function = increment;
	TEST_ASSERT(queue_destroy(q) == -1);
	TEST_ASSERT(queue_length(q) == -1);
	TEST_ASSERT(queue_enqueue(q, data) == -1);
	TEST_ASSERT(queue_dequeue(q, data) == -1);
	TEST_ASSERT(queue_delete(q, data) == -1);
	TEST_ASSERT(queue_iterate(q, function) == -1);

	// data is null
	q = queue_create();
	data = NULL;
	TEST_ASSERT(queue_enqueue(q, data) == -1);
	TEST_ASSERT(queue_dequeue(q, data) == -1);
	TEST_ASSERT(queue_delete(q, data) == -1);

	// function is null
	function = NULL;
	TEST_ASSERT(queue_iterate(q, function) == -1);

	// queue is empty
	TEST_ASSERT(queue_dequeue(q, data) == -1);

	// queue is not empty
	int a;
	queue_enqueue(q, &a);
	TEST_ASSERT(queue_destroy(q) == -1);

	// data not found
	int b;
	TEST_ASSERT(queue_delete(q, &b) == -1);

	queue_dequeue(q, data);
	queue_destroy(q);
}

# define NUM_TESTS 5
# define NUM_TRIALS 2 
char* tests[NUM_TESTS] = {"create", "enqueue", "length", "delete", "iterate"};
function_t testFunction[NUM_TESTS] = {&test_create, &test_enqueue, &test_length, &test_delete, &test_iterate};
/// have all test cases run through at least 2 iterations of action/inverse
/// and have one with all errors/edge cases

void runTest(int i) {
	if (i < 0 || i >= NUM_TESTS) {
		return;
	}

	fprintf(stderr, "*** TEST %s ***\n", tests[i]);

	for (int j = 0; j < NUM_TRIALS; j++) {
		q = queue_create();
		testFunction[i]();
		queue_destroy(q);
	}
}

int main(void)
{
	for (int i = 0; i < NUM_TESTS; i++) {
		runTest(i);
	}

	test_edge_cases();

	return 0;
}