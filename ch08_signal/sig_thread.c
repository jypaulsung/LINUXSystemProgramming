#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#define THREAD_MAIN // Enable signal handling in the main thread
/*
#define THREAD_1     // Uncomment to enable signal handling in Thread1
#define THREAD_2     // Uncomment to enable signal handling in Thread2
*/

// Signal handler for SIGINT (Ctrl+C)
void SigIntHandler(int signo)
{
    printf("Received a SIGINT signal by thread %d\n", pthread_self()); // Print the thread ID
    printf("Terminate this process\n");

    exit(0); // Exit the process cleanly
}

// Function for Thread1
void Thread1(void *dummy)
{
#ifdef THREAD_1
    signal(SIGINT, SigIntHandler); // Set up SIGINT handler for Thread1 if THREAD_1 is defined
#endif

    while (1) // Infinite loop to keep the thread running
        ;
}

// Function for Thread2
void Thread2(void *dummy)
{
#ifdef THREAD_2
    signal(SIGINT, SigIntHandler); // Set up SIGINT handler for Thread2 if THREAD_2 is defined
#endif

    while (1) // Infinite loop to keep the thread running
        ;
}

// Main function to create threads and set up signal handling
main()
{
    pthread_t tid1, tid2; // Thread IDs for Thread1 and Thread2

    // Create Thread1
    if (pthread_create(&tid1, NULL, (void *)Thread1, NULL) < 0) {
        perror("pthread_create"); // Print error if thread creation fails
        exit(1);
    }

    // Create Thread2
    if (pthread_create(&tid2, NULL, (void *)Thread2, NULL) < 0) {
        perror("pthread_create"); // Print error if thread creation fails
        exit(1);
    }

    // Print the created thread IDs
    printf("Create two threads: tid1=%d, tid2=%d\n", tid1, tid2);
    printf("Main thread: tid=%d\n", pthread_self()); // Print the main thread ID

#ifdef THREAD_MAIN
    signal(SIGINT, SigIntHandler); // Set up SIGINT handler for the main thread if THREAD_MAIN is defined
#endif

    printf("Press ^C to quit\n");

    for (; ;) // Main thread waits indefinitely for a signal
        pause(); // Suspend the process until a signal is received
}
