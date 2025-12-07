#include <stdio.h>
#include <signal.h>

void SigIntHandler(int signo)
{
    printf("Received a SIGINT signal\n");
    printf("Terminate this process\n");

    exit(0);
}

main()
{
    signal(SIGINT, SigIntHandler); // register a signal handler
    // takes two arguments: signal number and the pointer to the handler function

    printf("Press ^C to quit\n");

    for ( ; ; ) { // loop runs indefinitely
        pause(); // program sleeps until a signal is received
    }
}