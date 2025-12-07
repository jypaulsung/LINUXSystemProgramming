#include <stdio.h>
#include <signal.h>
#include <pthread.h> // pthread_create(), pthread_cancel(), pthread_join()

pthread_t	ThreadId[2]; // array to store the thread IDs of the two threads

void
SigIntHandler(int signo)
{
	int		i;

	printf("Received a SIGINT signal by thread %d\n", pthread_self()); // print the thread ID of the thread that received the SIGINT signal
	printf("Terminate other threads: tid=%d, %d\n", ThreadId[0], ThreadId[1]); 

	for (i = 0 ; i < 2 ; i++)  { // terminate the two threads
		if (pthread_cancel(ThreadId[i]))  {
			perror("pthread_cancel");
			exit(1);
		}
	}
	for (i = 0 ; i < 2 ; i++)  { // wait for the two threads to terminate
		if (pthread_join(ThreadId[i], NULL))  {
			perror("pthread_join");
			exit(1);
		}
	}
	printf("Threads terminated: tid=%d, %d\n", ThreadId[0], ThreadId[1]); // print the thread IDs of the two threads that have been terminated

	exit(0);
}

void
Thread1(void *dummy)
{
	printf("Thread %d created.....\n", pthread_self());

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  { // enable thread cancellation using pthread_setcancelstate()
		perror("pthread_setcancelstate");
		pthread_exit(NULL);
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  { // set the thread cancellation type to PTHREAD_CANCEL_ASYNCHRONOUS using pthread_setcanceltype()
		perror("pthread_setcanceltype");
		pthread_exit(NULL);
	}

	while (1) // make the thread run indefinitely until it is cancelled
		;
}

void
Thread2(void *dummy)
{
	printf("Thread %d created.....\n", pthread_self());

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  {
		perror("pthread_setcancelstate");
		pthread_exit(NULL);
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  {
		perror("pthread_setcanceltype");
		pthread_exit(NULL);
	}

	while (1)
		;
}

main()
{
	// create two threads
	if (pthread_create(&ThreadId[0], NULL, (void *)Thread1, NULL) < 0)  { 
		perror("pthread_create");
		exit(1);
	}
	if (pthread_create(&ThreadId[1], NULL, (void *)Thread2, NULL) < 0)  {
		perror("pthread_create");
		exit(1);
	}

	signal(SIGINT, SigIntHandler); // register the signal handler SigIntHandler for the SIGINT signal

	printf("Press ^C to quit\n");

	for ( ; ; )
		pause();
}