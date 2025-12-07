#include <stdio.h>
#include <pthread.h> // pthread_create(), pthread_exit(), pthread_join()


void
PrintMsg(char *msg) // print the message passed as an argument
{
	printf("%s", msg);

	pthread_exit(NULL); // terminate the calling thread
}

main()
{
	pthread_t	tid1, tid2;
	char		*msg1 = "Hello, ";
	char		*msg2 = "World!\n";


	if (pthread_create(&tid1, NULL, (void *)PrintMsg, (void *)msg1) < 0)  { // create a new thread with the identifier tid1
        // the new thread will execute the PrintMsg() with msg1 as its argument
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, (void *)PrintMsg, (void *)msg2) < 0)  {
		perror("pthread_create");
		exit(1);
	}

	printf("Threads created: tid=%d, %d\n", tid1, tid2); // print the thread identifiers
	
	if (pthread_join(tid1, NULL) < 0)  { // wait for the thread with the identifier tid1 to terminate
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  { // wait for the thread with the identifier tid2 to terminate
		perror("pthread_join");
		exit(1);
	}

	printf("Threads terminated: tid=%d, %d\n", tid1, tid2);
}