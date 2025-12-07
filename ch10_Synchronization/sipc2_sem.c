#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include "semlib.h"

#define SEM_KEY 0x1234

int main() {
    int shmid, semid;
    char *ptr;

    // Initialize semaphore
    if ((semid = semInit(SEM_KEY)) < 0) {
        perror("semInit");
        exit(1);
    }

    // Allocate shared memory segment
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget");
        exit(1);
    }
    // Attach shared memory segment
    if ((ptr = shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // Write request to shared memory
    sprintf(ptr, "This is a request from %d.", getpid());
    printf("Sent a request.....\n");

    // Signal semaphore to notify the server
    if (semPost(semid) < 0) {
        perror("semPost");
        exit(1);
    }

    // Wait for reply
    if (semWait(semid) < 0) {
        perror("semWait");
        exit(1);
    }

    // Read and print reply
    printf("Received reply: %s\n", ptr);

    return 0;
}
