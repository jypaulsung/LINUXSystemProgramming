#include <stdio.h>
#include <signal.h>

void SigUsrHandler(int signo) 
{
    if (signo == SIGUSR1) {
        printf("Received a SIGUSR1 signal\n");
    }
    else if (signo == SIGUSR2) {
        printf("Received a SIGUSR2 signal\n");
    }
    else {
        printf("Received unknown signal\n");
        printf("Terminating this process\n");
        exit(0);
    }
}

main()
{
    // register a signal handler for the SIGUSR1 signal
    if (signal(SIGUSR1, SigUsrHandler) == SIG_ERR) { 
        // if signal() returns SIG_ERR, it means that the signal handler registration failed
        perror("signal");
        exit(1);
    }

    // register a signal handler for the SIGUSR2 signal
    if (signal(SIGUSR2, SigUsrHandler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    for ( ; ; ) {
        pause();
    }
}

// ps aux | grep "process_name" - find the process id of a process
// kill -SIGUSR1 pid - send a SIGUSR1 signal to the process with the given pid