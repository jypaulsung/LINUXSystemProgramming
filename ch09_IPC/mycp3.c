#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h> // For memory mapping functions
#include <sys/stat.h> // For fstat to retrieve file information
#include <fcntl.h>    // For open system call

main(int argc, char *argv[])
{
    int fdin, fdout;              // File descriptors for input and output files
    char *src, *dst;              // Pointers for memory-mapped source and destination
    struct stat statbuf;          // Structure to hold file information

    // Check if the correct number of arguments are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source destination\n", argv[0]);
        exit(1);
    }

    // Open the source file in read-only mode
    if ((fdin = open(argv[1], O_RDONLY)) < 0) {
        perror("open"); // Print error if open fails
        exit(1);
    }

    // Open or create the destination file with read-write permissions, and truncate it
    if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror("open"); // Print error if open fails
        exit(1);
    }

    // Retrieve information about the source file (e.g., file size)
    if (fstat(fdin, &statbuf) < 0) {
        perror("fstat"); // Print error if fstat fails
        exit(1);
    }

    // Extend the destination file size to match the source file size
    if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) < 0) {
        perror("lseek"); // Print error if lseek fails
        exit(1);
    }
    write(fdout, "", 1); // Write a single byte to ensure the file size is updated

    // Memory-map the source file into memory for reading
    if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) 
        == (caddr_t) -1) {
        perror("mmap"); // Print error if mmap fails
        exit(1);
    }

    // Memory-map the destination file into memory for writing
    if ((dst = mmap(0, statbuf.st_size, PROT_WRITE, MAP_SHARED, fdout, 0)) 
        == (caddr_t) -1) {
        perror("mmap"); // Print error if mmap fails
        exit(1);
    }

    // Copy the contents of the source file to the destination file
    memcpy(dst, src, statbuf.st_size);

    // Close the file descriptors for the input and output files
    close(fdin);
    close(fdout);
}
