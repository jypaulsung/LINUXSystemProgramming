#include <stdio.h>
#include <signal.h>

void
SigAlarmHandler(int signo)
{
	/* nothing to do, just return to wake up the pause */
	return;
}

unsigned int
mysleep(unsigned int nsecs)
{
	if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  {
		return nsecs;
	}

	alarm(nsecs); // set the alarm to send a SIGALRM signal in nsecs seconds

	pause(); // suspend the process until a signal is received

	return alarm(0); // cancel any pending alarms and return the number of seconds remaining until the alarm was due to go off
	// if it returns 0, it means that the alarm has already gone off
}

main()
{
	printf("Wait for 5 seconds...\n");

	mysleep(5); 
}