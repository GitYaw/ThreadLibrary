/*
 * Producer/consumer test
 *
 * A producer produces x values in a shared buffer, while a consume consumes y
 * of these values. x and y are always less than the size of the buffer but can
 * be different. The synchronization is managed through two semaphores.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <sem.h>
#include <uthread.h>

#define BUFFER_SIZE	16
#define MAXCOUNT	1000

struct test4 {
	sem_t empty;
	sem_t full;
	sem_t mutex;
	size_t size, head, tail, maxcount;
	unsigned int prod_seed, cons_seed;
	unsigned int buffer[BUFFER_SIZE];
};

#define clamp(x, y) (((x) <= (y)) ? (x) : (y))

static void consumer(void* arg)
{
	struct test4 *t = (struct test4*)arg;
	size_t out = 0;

	while (out < t->maxcount - 1) {
		size_t i, n = rand_r(&t->cons_seed) % BUFFER_SIZE + 1;

		n = clamp(n, t->maxcount - out - 1);
		printf("Consumer wants to get %zu items out of buffer...\n", n);
		for (i = 0; i < n; i++) {
			sem_down(t->empty);
			out = t->buffer[t->tail];
			printf("Consumer is taking %zu out of buffer\n", out);
			t->tail = (t->tail + 1) % BUFFER_SIZE;
			sem_down(t->mutex);
			t->size--;
			sem_up(t->mutex);
			sem_up(t->full);
		}
	}
}

static void producer(void* arg)
{
	struct test4 *t = (struct test4*)arg;
	size_t count = 0;

	uthread_create(consumer, arg);

	while (count < t->maxcount) {
		size_t i, n = rand_r(&t->prod_seed) % BUFFER_SIZE + 1;
		n = clamp(n, t->maxcount - count);

		printf("Producer wants to put %zu items into buffer...\n", n);
		for (i = 0; i < n; i++) {
			sem_down(t->full);
			printf("Producer is putting %zu into buffer\n", count);
			t->buffer[t->head] = count++;
			t->head = (t->head + 1) % BUFFER_SIZE;
			sem_down(t->mutex);
			t->size++;
			sem_up(t->mutex);
			sem_up(t->empty);
		}
	}
}

static unsigned int get_argv(char *argv)
{
	long int ret = strtol(argv, NULL, 0);
	if (ret == LONG_MIN || ret == LONG_MAX) {
		perror("strtol");
		exit(1);
	}
	return ret;
}

int main(int argc, char **argv)
{
	struct test4 t;
	unsigned int maxcount = MAXCOUNT;

	t.cons_seed = 1;
	t.prod_seed = 2;

	if (argc > 1)
		maxcount = get_argv(argv[1]);
	if (argc > 2)
		t.cons_seed = get_argv(argv[2]);
	if (argc > 3)
		t.prod_seed = get_argv(argv[3]);

	t.size = t.head = t.tail = 0;
	t.maxcount = maxcount;

	t.mutex = sem_create(1);
	t.empty = sem_create(0);
	t.full = sem_create(BUFFER_SIZE);

	uthread_run(false, producer, &t);

	sem_destroy(t.empty);
	sem_destroy(t.full);
	sem_destroy(t.mutex);

	return 0;
}
