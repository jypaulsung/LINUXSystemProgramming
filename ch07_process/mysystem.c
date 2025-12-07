#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int mysystem(char *cmd) {
    pid_t pid = fork(); // create a child process
    // check for fork failure
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) { // child process
        char *argv[] = {"/bin/sh", "-c", cmd, NULL}; // command to be executed by the shell
													 // "/bin/sh": shell, "-c": tells the shell to execute the following command
													 // cmd: the actual command, NULL: terminator
        execv("/bin/sh", argv); // replace the current child process with a new process running the shell with the command specified in argv
        perror("execv"); // print error message if execv fails
        exit(EXIT_FAILURE); // exit with failure status
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) { // wait for the child process to terminate
            perror("waitpid"); // print error message if waitpid fails
            return -1;
        }
        if (WIFEXITED(status)) { // check if the child process terminated normally
            return WEXITSTATUS(status); // return the exit status of the child process
        } else {
            return -1; // return -1 if the child process did not terminate normally
        }
    }
}

int main() {
    char cmd[256];
    printf("Enter a command to execute: ");
    fgets(cmd, sizeof(cmd), stdin);

    size_t len = strlen(cmd);
    if (len > 0 && cmd[len - 1] == '\n') {
        cmd[len - 1] = '\0'; // replace the newline character at the end with a null terminator
    }

    int result = mysystem(cmd);
    printf("Command executed with exit status: %d\n", result);
}
