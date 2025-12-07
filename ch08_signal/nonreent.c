#include <stdio.h>
#include <signal.h>
#include <pwd.h> // Provides functions to access the password database, like getpwnam()

// Signal handler for the SIGALRM signal
void
MyAlarmHandler(int signo)
{
    struct passwd *rootptr;

    // Re-register the signal handler for SIGALRM to ensure it remains effective for future signals
    signal(SIGALRM, MyAlarmHandler);
    alarm(1); // Reset the alarm to trigger in 1 second

    printf("in signal handler\n");

    // Retrieve the password entry for the "root" user
    // This uses a non-reentrant function (getpwnam), which accesses shared static memory
    if ((rootptr = getpwnam("root")) == NULL) {
        perror("getpwnam"); // Print error if getpwnam fails
        exit(1);
    }

    return; // Return from the signal handler
}

// Main function
main()
{
    struct passwd *ptr;

    // Register the signal handler for SIGALRM
    signal(SIGALRM, MyAlarmHandler);
    alarm(1); // Set the alarm to go off in 1 second

    for ( ; ; ) { // Infinite loop
        // Retrieve the password entry for the "cjs" user
        // This also uses the non-reentrant getpwnam function
        if ((ptr = getpwnam("cjs")) == NULL) {
            perror("getpwnam");
            exit(1);
        }

        // Check if the returned entry is corrupted
        if (strcmp(ptr->pw_name, "cjs") != 0) {
            printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
            exit(0); // Exit if data corruption is detected
        }
    }
}
