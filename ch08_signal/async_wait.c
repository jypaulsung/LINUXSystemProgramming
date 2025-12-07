#include <stdio.h>
#include <sys/types.h> // data type definitions
#include <unistd.h> // fork()
#include <sys/wait.h> // wait()
#include <signal.h>

void SigChldHandler(int signo)
{
    if (signo == SIGCHLD) {
        printf("Received a SIGCHLD signal\n");
        printf("The child process has been terminated\n");
        printf("Now the parent process will terminate\n");
        exit(0); // exit the program
    }
    else {
        printf("Received an unknown signal\n");
        exit(0);
    }
}

main()
{
    pid_t pid;
    int status;

    // Register a signal handler for the SIGCHLD signal
    if (signal(SIGCHLD, SigChldHandler) == SIG_ERR) {
        perror("signal"); // if signal() returns SIG_ERR, it means that the signal handler registration failed
        exit(1); // exit the program
    }
    
    // Create a child process
    if ((pid = fork()) < 0) {
        perror("fork"); // if fork() returns -1, it means that the child process creation failed
        exit(1); // exit the program
    }
    else if (pid == 0) { // child process
        printf("A child process has been created\n");
        sleep(2); // sleep for 2 seconds and then terminate the child process
        exit(0);
    }
    
    while(1) { // this loop is outside the "else if (pid == 0)" block, so it is executed only by the parent process
        pause(); // wait for a signal and then return
    }
}