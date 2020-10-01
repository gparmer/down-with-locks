/*
 * The imposter in the ranks. This is the stealthy COUNTER-revolution
 * where we use tokens to ensure bounded-wait! Little does the
 * revolution know that it has a fundamental rot in its midst: the ABA
 * problem. Their system will collapse, and when it does, we'll be
 * ready to provide the mutually exclusive order our data-structure
 * society requires.
 */

#include <ps_arch_x86_common.h>

struct ticket_lock {
	unsigned long ticket;   /* which ticket number will we pull? */
	unsigned long serving;	/* which ticket number is the lock serving? */
};

void
ticket_init(struct ticket_lock *l)
{
	*l = (struct ticket_lock) {
		.ticket  = 0,
		.serving = 0
	};
}

void
ticket_take(struct ticket_lock *l)
{
	unsigned long ticket;

	ticket = (unsigned long)ps_faa(&l->ticket, 1);
	while (ticket != ps_load(&l->serving)) ;
}

void
ticket_release(struct ticket_lock *l)
{
	l->serving++;
}
