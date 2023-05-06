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

static void increment(queue_t q, void *data)
{
    int* a = (int*) data;

    if (*a == 3) {
        // queue_delete(q, data);
	} else {
        *a += 1;
	}
}

void test_delete() {
	return;
}

void test_iterate() {
	int ar[] = {0, 2, 4, 6};
	for (int i = 0; i < sizeof(ar) / sizeof(int); i++) {
		queue_enqueue(q, ar + i);
	}

	queue_iterate(q, increment);
	TEST_ASSERT(ar[0] == 1);
	TEST_ASSERT(ar[2] == 5);

	void* data;
	for (int i = 0; i < sizeof(ar) / sizeof(int); i++) {
		queue_dequeue(q, &data);
	}
}

	queue_t q = queue_create();
	int a = 1;
	int b = 2;
	int c = 3;
	queue_enqueue(q, &a);
	queue_enqueue(q, &b);
	queue_enqueue(q, &c);

	queue_iterate(q, print);

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

	return 0;
}