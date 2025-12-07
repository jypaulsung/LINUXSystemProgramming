#include <stdio.h>
#include "synclib.h" // Include synchronization library for TELL_WAIT, TELL_CHILD, WAIT_CHILD, etc.

#define NLOOPS 5 // Number of iterations for communication between parent and child

main()
{
    int i;            // Loop counter for iterations
    pid_t pid;        // Process ID for the forked child process

    TELL_WAIT();      // Initialize synchronization primitives for parent-child communication

    // Fork a child process
    if ((pid = fork()) < 0) {
        perror("fork"); // Print error if fork fails
        exit(1);
    }
    else if (pid > 0) { // Parent process block
        for (i = 0; i < NLOOPS; i++) {
            TELL_CHILD(); // Notify the child process to proceed
            printf("Parent: Tell to child\n");

            printf("Parent: Wait for child to tell\n");
            WAIT_CHILD(); // Wait for a signal from the child process
        }
    }
    else { // Child process block
        for (i = 0; i < NLOOPS; i++) {
            printf("Child: Wait for parent to tell\n");
            WAIT_PARENT(); // Wait for a signal from the parent process

            TELL_PARENT(); // Notify the parent process to proceed
            printf("Child: Tell to parent\n");
        }
    }
}
