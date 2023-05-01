/*
 * Sieve test for finding prime numbers
 *
 * A producer thread (source) creates numbers and inserts them into a pipeline,
 * a consumer thread (sink) gets prime numbers from the end of the pipeline. The
 * pipeline consists of filtering thread, added dynamically each time a new
 * prime number is found and which filters out subsequent numbers that are
 * multiples of that prime.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sem.h>
#include <uthread.h>

#define MAXPRIME 1000

struct channel {
	int value;
	sem_t produce;
	sem_t consume;
};

struct filter {
	struct channel *left;
	struct channel *right;
	unsigned int prime;
	struct filter *next;
};

static unsigned int max = MAXPRIME;

/* Producer thread: produces all numbers, from 2 to max */
static void source(void *arg)
{
	struct channel *c = (struct channel*) arg;
	size_t i;

	for (i = 2; i <= max; i++) {
		c->value = i;
		sem_up(c->consume);
		sem_down(c->produce);
	}

	/* mark completion */
	c->value = -1;
	sem_up(c->consume);
	sem_down(c->produce);
}

/* Filter thread */
static void filter(void *arg)
{
	struct filter *f = (struct filter*) arg;
	int value;

	while (1) {
		sem_down(f->left->consume);
		value = f->left->value;
		sem_up(f->left->produce);
		if ((value == -1) || (value % f->prime != 0)) {
			f->right->value = value;
			sem_up(f->right->consume);
			sem_down(f->right->produce);
		}
		if (value == -1)
			break;
	}

	sem_destroy(f->left->produce);
	sem_destroy(f->left->consume);
	free(f->left);
	free(f);
}

/* Consumer thread */
static void sink(void *arg)
{
	struct channel *init_p, *p;
	int value;
	struct filter *f_head = NULL;
	(void)arg;

	init_p = malloc(sizeof(*init_p));

	p = init_p;
	p->produce = sem_create(0);
	p->consume = sem_create(0);

	uthread_create(source, p);

	while (1) {
		struct filter *f;

		sem_down(p->consume);
		value = p->value;
		sem_up(p->produce);

		if (value == -1)
			break;

		printf("%d is prime.\n", value);

		f = malloc(sizeof(*f));
		f->left = p;
		f->prime = value;
		f->next = NULL;

		p = malloc(sizeof(*p));
		p->produce = sem_create(0);
		p->consume = sem_create(0);

		f->right = p;

		uthread_create(filter, f);

		if (f_head)
			f->next = f_head;
		f_head = f;
	}

	sem_destroy(p->produce);
	sem_destroy(p->consume);
	free(p);
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
	if (argc > 1)
		max = get_argv(argv[1]);

	uthread_run(false, sink, NULL);

	return 0;
}
