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
    if (semInitValue(semid, 0) < 0) { // Start with 0 to block the client initially
        perror("semInitValue");
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

    // Wait for request
    if (semWait(semid) < 0) {
        perror("semWait");
        exit(1);
    }

    // Process request
    printf("Received request: %s\n", ptr);
    sprintf(ptr, "This is a reply from %d.", getpid()); // Write reply to shared memory
    printf("Replied.\n");

    // Signal semaphore to notify the client
    if (semPost(semid) < 0) {
        perror("semPost");
        exit(1);
    }

    // Mark shared memory segment for deletion
    sleep(1); // Ensure the client has time to read the reply
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}
