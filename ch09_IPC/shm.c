#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define ARRAY_SIZE 40000       // Size of the global array
#define MALLOC_SIZE 100000      // Size for the dynamically allocated memory
#define SHM_SIZE 100000         // Size for the shared memory segment
#define SHM_MODE (SHM_R | SHM_W) // Shared memory permissions (read and write)

char Array[ARRAY_SIZE];         // Global array with fixed size allocated in data segment

main()
{
    int shmid;               // Shared memory ID
    char *ptr, *shmptr;      // Pointers for dynamic memory and shared memory

    // Allocate memory dynamically with malloc
    if ((ptr = (char *)malloc(MALLOC_SIZE)) == NULL) {
        perror("malloc"); // Print error if malloc fails
        exit(1);
    }

    // Create a shared memory segment
    if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget"); // Print error if shmget fails
        exit(1);
    }

    // Attach the shared memory segment to the process's address space
    if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat"); // Print error if shmat fails
        exit(1);
    }

    // Print the memory locations for different segments
    printf("Array[] from %p to %p\n", &Array[0], &Array[ARRAY_SIZE]); // Global array
    printf("Stack around %p\n", &shmid);                             // Stack memory location
    printf("Malloced from %p to %p\n", ptr, ptr + MALLOC_SIZE);       // Dynamically allocated memory
    printf("Shared memory attached from %p to %p\n", shmptr, shmptr + SHM_SIZE); // Shared memory segment

    // Mark the shared memory segment for deletion
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl"); // Print error if shmctl fails
        exit(1);
    }
}
