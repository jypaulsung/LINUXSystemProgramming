#include <stdio.h>
#include <signal.h>

static unsigned int	AlarmSecs; // static global variable to store the interval for the periodic alarm


void
SigAlarmHandler(int signo) // signal handler for the SIGALRM signal
{
	if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  { // resets the signal handler for the SIGALRM signal to SigAlarmHandler to ensure it continues to handle future SIGALRM signals
		perror("signal");
		exit(1);
	}

	alarm(AlarmSecs); // sets the alarm to go off in AlarmSecs seconds

	/* Do something */
	printf("."); 
	fflush(stdout); // flushes the output buffer to ensure that the dot is printed immediately

	return;
}

int
SetPeriodicAlarm(unsigned int nsecs)
{
	if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  { // registers the signal handler SigAlarmHandler for the SIGALRM signal
		return -1;
	}

	AlarmSecs = nsecs; // set the global variable AlarmSecs to nsecs

	alarm(nsecs); // sets the alarm to go off in nsecs seconds

	return 0;
}

main()
{
	printf("Doing something every one seconds\n"); 

	SetPeriodicAlarm(1); // sets the alarm to go off every one second

	for ( ; ; )
		pause(); // suspend the process until a signal is received
}