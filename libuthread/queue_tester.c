#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "queue.h"

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
	queue_t q = queue_create();
	TEST_ASSERT(q != NULL);

	queue_destroy(q);
}

void test_enqueue() {
	queue_t q = queue_create();
	int a, b;

	queue_enqueue(q, &a);
	queue_enqueue(q, &b);

	void* data;

	queue_dequeue(q, &data);
	TEST_ASSERT(data == &a);
	queue_dequeue(q, &data);
	TEST_ASSERT(data == &b);

	queue_destroy(q);
}

void test_length() {
	queue_t q = queue_create();
	int a;
	for (int i = 0; i < 3; i++) {
		queue_enqueue(q, &a);
	}
	void* data;
	queue_dequeue(q, &data);

	int length = queue_length(q);
	TEST_ASSERT(length == 2);

	queue_destroy(q);
}

void print(queue_t q, void* d) {
	printf("%d\n", *(int*)d);
}

void test_iterate() {
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

# define NUM_TESTS 4
typedef void (*function_t)();  
char* tests[NUM_TESTS] = {"create", "enqueue", "length", "iterate"};
function_t testFunction[NUM_TESTS] = {&test_create, &test_enqueue, &test_length, &test_iterate};

void runTest(int i) {
	if (i < 0 || i >= NUM_TESTS) {
		return;
	}

	fprintf(stderr, "*** TEST %s ***\n", tests[i]);

	testFunction[i]();
}

int main(void)
{
	for (int i = 0; i < NUM_TESTS; i++) {
		runTest(i);
	}

	return 0;
}