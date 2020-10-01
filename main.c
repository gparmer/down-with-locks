/*
 * One of these has to be defined: choose a stage of the lock-free
 * revolution
 */
#define BURN_IT_DOWN 1
//#define WE_HAVE_A_PLAN 1
//#define PROFESSIONALS_TAKE_OVER 1

#include <ps_arch_x86_common.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

struct node {
	int          value;
	struct node *next;
};

volatile struct node *head = NULL;

struct node *
node_alloc(int value)
{
	struct node *n = malloc(sizeof(struct node));

	if (!n) return NULL;
	*n = (struct node) {
		.value = value,
		.next  = NULL
	};

	return n;
}

void
node_free(struct node *n)
{
	free(n);
}

/* just simple type conversions */
int
revolucion_cas(volatile struct node **head, struct node *guess, struct node *newval)
{
	return ps_cas((unsigned long *)head, (unsigned long)guess, (unsigned long)newval);
}

void
stack_push(int value)
{
	struct node *n = node_alloc(value);
	struct node *first;

	assert(n);
try_again:
	first   = head;
	n->next = first;
#ifdef BURN_IT_DOWN
	head    = n;
#elif  WE_HAVE_A_PLAN
	if (!revolucion_cas(&head, first, n)) {
		printf("x-( ");
	}
#elif  PROFESSIONALS_TAKE_OVER
	if (!revolucion_cas(&head, first, n)) goto try_again;
#endif
}

int
stack_pop(void)
{
	struct node *first;
	int value;

try_again:
	if (!head) return -1;
	first = head;
	value = first->value;

#ifdef BURN_IT_DOWN
	head = first->next;
#elif  WE_HAVE_A_PLAN
	if (!revolucion_cas(&head, first, first->next)) return -1;
#elif  PROFESSIONALS_TAKE_OVER
	if (!revolucion_cas(&head, first, first->next)) goto try_again;
#endif
	node_free(first);

	return value;
}

void
panic(char *msg)
{
	if (errno) perror(msg);
	else       printf("%s", msg);
	exit(EXIT_FAILURE);
}

#define ITER 1000000

void *
popper_revolutionary(void *d)
{
	int i;
	(void)d;

	for (i = 0; i < ITER; i++) {
		stack_pop();
	}

	return NULL;
}

void *
pusher_revolutionary(void *d)
{
	int i;
	(void)d;

	for (i = 0; i < ITER; i++) {
		stack_push(i);
	}

	return NULL;
}

int
main(int argc, char *argv[])
{
	pthread_t popper, pusher;
	(void)argc; (void)argv;

	if (pthread_create(&pusher, NULL, pusher_revolutionary, NULL)) panic("pthread_create");
	if (pthread_create(&popper, NULL, popper_revolutionary, NULL)) panic("pthread_create");
	pthread_join(pusher, NULL);
	pthread_join(popper, NULL);

	printf("The revolution will be lock free!\n");

	return 0;
}




















/*
 * The lock COUNTER revolution to overthrow the lockless
 * revolution is in ticket.c. Now we can have locks *with bounded wait
 * guarantees!!!!*, lurking in the shadows.
 */
