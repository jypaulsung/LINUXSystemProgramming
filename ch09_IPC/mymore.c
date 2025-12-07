#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() 
{
    int fd[2];           // File descriptors for the pipe: fd[0] is read-end, fd[1] is write-end
    pid_t child;         // Process ID for the child process

    // Create a pipe for inter-process communication
    if (pipe(fd) == -1) {
        perror("pipe"); // Print error if pipe creation fails
        exit(1);
    }
 
    // Fork a child process
    child = fork();
    if (child == (pid_t)(-1)) {
        perror("fork"); // Print error if fork fails
        exit(1);
    }
    else if (child == (pid_t)0) { // Child process block
        close(1);       // Close the standard output (stdout)
        close(fd[0]);   // Close the read-end of the pipe in the child process
        
        // Duplicate the write-end of the pipe to stdout
        if (dup(fd[1]) == -1) {
            perror("dup"); // Print error if duplication fails
            exit(1);
        }

        // Execute the "ls -l" command, sending its output to the write-end of the pipe
        if ((execlp("ls", "ls", "-l", NULL)) == -1) {
            perror("execlp"); // Print error if execlp fails
            exit(1);
        }
    } 
    else { // Parent process block
        close(0);       // Close the standard input (stdin)
        close(fd[1]);   // Close the write-end of the pipe in the parent process
        
        // Duplicate the read-end of the pipe to stdin
        if (dup2(fd[0], 0) == -1) {
            perror("dup"); // Print error if duplication fails
            exit(1);
        }

        // Execute the "more" command, reading input from the read-end of the pipe
        if ((execlp("more", "more", NULL)) == -1) {
            perror("execlp"); // Print error if execlp fails
            exit(1);
        }
    }
 
    return 0; // Return success
}
