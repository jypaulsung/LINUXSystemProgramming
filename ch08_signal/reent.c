#include <stdio.h>
#include <signal.h>     // For signal handling functions
#include <pwd.h>        // For user password database access
#include <string.h>     // For string comparison functions
#include <stdlib.h>     // For exit()

// Signal handler function for SIGALRM
void MyAlarmHandler(int signo)
{
    struct passwd pwd;       // Local struct to store user password database entry
    struct passwd *result;   // Pointer to the result of the query
    char buf[1024];          // Buffer to hold user data, avoiding use of static memory

    // Re-register the signal handler for SIGALRM
    signal(SIGALRM, MyAlarmHandler);

    // Set the alarm to go off in 1 second
    alarm(1);

    printf("in signal handler\n");

    // Use the reentrant function `getpwnam_r` to query the user "root"
    // `getpwnam_r` writes the results into the provided `pwd` structure and `buf` buffer,
    // ensuring thread-safe and reentrant behavior.
    if (getpwnam_r("root", &pwd, buf, sizeof(buf), &result) != 0 || result == NULL) {
        perror("getpwnam_r"); // Print error if the call fails
        exit(1);              // Exit on failure
    }

    return; // Return control to the caller
}

// Main function
int main()
{
    struct passwd pwd;       // Local struct to store user password database entry
    struct passwd *result;   // Pointer to the result of the query
    char buf[1024];          // Buffer to hold user data, avoiding use of static memory

    // Register the signal handler for SIGALRM
    signal(SIGALRM, MyAlarmHandler);

    // Set the alarm to go off in 1 second
    alarm(1);

    for ( ; ; ) { // Infinite loop
        // Use the reentrant `getpwnam_r` to query the user "cjs"
        // Avoids static memory, making the function safe for concurrent use
        if (getpwnam_r("cjs", &pwd, buf, sizeof(buf), &result) != 0 || result == NULL) {
            perror("getpwnam_r"); // Print error if the call fails
            exit(1);              // Exit on failure
        }

        // Compare the returned username with "cjs" to verify correctness
        if (strcmp(pwd.pw_name, "cjs") != 0) {
            printf("return value corrupted!, pw_name = %s\n", pwd.pw_name);
            exit(0); // Exit if corruption is detected
        }
    }
}
