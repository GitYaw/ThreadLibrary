#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

static struct itimerval timer;
struct sigaction sa;
static sigset_t block;

/*
	Install a signal handler that receives alarm signals (type SIGVTALRM)
	Configure a time which will fire an alarm (through a SIGVTALRM signal) 100 Hz
*/

// Timer interrupt handler
void handler(int signum) {
	if (signum == SIGVTALRM) {
		uthread_yield();
	}
}

void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &block, NULL);
}

void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &block, NULL);

}

void preempt_start(bool preempt)
{
	if (preempt) {
		// Creating the structure for new action and forcing current running thread to yield
		sa.sa_handler = handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sigaction(SIGVTALRM, &sa, NULL);

		// Setting an alarm/timer
		timer.it_value.tv_usec = 1000000 / HZ;
		timer.it_value.tv_sec = 0;
		timer.it_interval = timer.it_value;

		// Begin timer
		// ITIMER_VIRTUAL has value that is decremented when process is running
		// SIGVTALRM signal is generated for process when timer expires
		setitimer(ITIMER_VIRTUAL, &timer, NULL);
	}
}

void preempt_stop(void)
{
	// Restores previous timer 
	setitimer(ITIMER_VIRTUAL, &timer, NULL);

	// use SIG_DFL to terminate the process since it restores default behavior
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGVTALRM, &sa, NULL);
}
