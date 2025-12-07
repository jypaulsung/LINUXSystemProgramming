#include <stdio.h> // For printf
#include <sys/types.h> // For pid_t
#include <unistd.h> // For fork, pipe, read, write

#define	MAX_BUF	128

main()
{
    int		n, fd1[2], fd2[2];
    pid_t	pid;
    char	buf[MAX_BUF]; // Define buffer size

    if (pipe(fd1) < 0 || pipe(fd2) < 0)  { // Create pipe
        perror("pipe");
        exit(1);
    }

    if ((pid = fork()) < 0)  { // Create child process
        perror("fork");
        exit(1);
    }
    else if (pid == 0)  { // Child process
        close(fd1[1]); // Close write end of first pipe
        close(fd2[0]); // Close read end of second pipe

        printf("Child : Wait for parent to send data\n");
        if ((n = read(fd1[0], buf, MAX_BUF)) < 0)  { // Read data from the pipe into the buffer
            perror("read");
            exit(1);
        }
        printf("Child : Received data from parent: "); 
        fflush(stdout); // Flush the output buffer
        write(STDOUT_FILENO, buf, n); // Write the received data to standard output

        // Send reply to parent
        strcpy(buf, "Hello from child!\n");
        printf("Child: Send data to parent\n");
        write(fd2[1], buf, strlen(buf)+1);

        close(fd1[0]); // Close read end of first pipe after receiving data
        close(fd2[1]); // Close write end of second pipe after sending data
        exit(0);    
    }
    else  { // Parent process
        close(fd1[0]); // Close read end of first pipe
        close(fd2[1]); // Close write end of second pipe

        strcpy(buf, "Hello, World!\n"); // Copy the string into the buffer
        printf("Parent: Send data to child\n");
        write(fd1[1], buf, strlen(buf)+1); // Write the data from the buffer to the pipe, +1 to include NULL terminator
    
        // Wait for reply from child
        if ((n = read(fd2[0], buf, MAX_BUF)) < 0)  { // Read data from the pipe into the buffer
            perror("read");
            exit(1);
        }
        printf("Parent: Received reply from child: ");
        fflush(stdout); // Flush the output buffer
        write(STDOUT_FILENO, buf, n); // Write the received data to standard output

        close(fd1[1]); // Close write end of first pipe after sending data
        close(fd2[0]); // Close read end of second pipe after receiving data
        wait(NULL); // Wait for child process to finish
        printf("Parent: Child process has finished\n");
    }

    exit(0);
}