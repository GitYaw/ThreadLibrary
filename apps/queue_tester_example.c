#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

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

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_queue_length(void)
{
	int *data1 = malloc(sizeof(int));
    int *data2 = malloc(sizeof(int));
	*data1 = 3;
    *data2 = 5;

    queue_t q;

    fprintf(stderr, "*** TEST queue_length ***\n");

    q = queue_create();
    TEST_ASSERT(queue_length(q) == 0);
    queue_enqueue(q, data1);
    TEST_ASSERT(queue_length(q) == 1);
    queue_enqueue(q, data2);
    TEST_ASSERT(queue_length(q) == 2);
	queue_dequeue(q, (void**)(&data1));
    TEST_ASSERT(queue_length(q) == 1);
    queue_dequeue(q, (void**)(&data2));
    TEST_ASSERT(queue_length(q) == 0);

	free(data1);
	free(data2);
}


/* Queue delete */
void test_queue_delete(void)
{
    int *data1 = (int*)malloc(sizeof(int));
    int *data2 = (int*)malloc(sizeof(int));
    int *data3 = (int*)malloc(sizeof(int));
    queue_t q;

    fprintf(stderr, "*** TEST queue_delete ***\n");

    *data1 = 1;
    *data2 = 2;
    *data3 = 3;

    q = queue_create();
    queue_enqueue(q, data1);
    queue_enqueue(q, data2);
    queue_enqueue(q, data3);

    // Test deletion of middle node
    TEST_ASSERT(queue_delete(q, data2) == 0);
    TEST_ASSERT(queue_length(q) == 2);

    // Test deletion of front node
    TEST_ASSERT(queue_delete(q, data1) == 0);
    TEST_ASSERT(queue_length(q) == 1);

    // Test deletion of back node
    TEST_ASSERT(queue_delete(q, data3) == 0);
    TEST_ASSERT(queue_length(q) == 0);
}

/* Callback function that increments items */
static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 42, 3, 4};
    size_t i;

	fprintf(stderr, "*** TEST test_iterator ***\n");


    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 4);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_queue_length();
	test_queue_delete();
	test_iterator();

	return 0;
}
