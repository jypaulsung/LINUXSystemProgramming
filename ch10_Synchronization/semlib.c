#include <stdio.h>
#include "semlib.h" // Include the header file for semaphore functions

// Function to create and initialize a semaphore
int semInit(key_t key)
{
    int semid; // Semaphore ID

    // Create a semaphore set with one semaphore, key is provided as input
    if ((semid = semget(key, 1, 0600 | IPC_CREAT)) < 0) {
        perror("semget"); // Print error if semaphore creation fails
        return -1;
    }

    return semid; // Return the semaphore ID
}

// Function to set the initial value of the semaphore
int semInitValue(int semid, int value)
{
    union semun {
        int val; // Value for SETVAL operation
    } semun;

    semun.val = value; // Set the desired semaphore value

    // Set the value of the semaphore in the set
    if (semctl(semid, 0, SETVAL, semun) < 0) {
        perror("semctl"); // Print error if the value setting fails
        return -1;
    }

    return semid; // Return the semaphore ID
}

// Function to perform a wait (P operation) on the semaphore
int semWait(int semid)
{
    struct sembuf semcmd; // Semaphore operation structure

    semcmd.sem_num = 0;       // Semaphore index
    semcmd.sem_op = -1;       // Decrement semaphore value (P operation)
    semcmd.sem_flg = SEM_UNDO; // Ensure semaphore is undone if process terminates

    // Perform the semaphore operation (wait)
    if (semop(semid, &semcmd, 1) < 0) {
        perror("semop"); // Print error if the operation fails
        return -1;
    }

    return 0; // Return success
}

// Function to perform a try-wait (non-blocking P operation) on the semaphore
int semTryWait(int semid)
{
    struct sembuf semcmd; // Semaphore operation structure

    semcmd.sem_num = 0;                // Semaphore index
    semcmd.sem_op = -1;                // Decrement semaphore value (P operation)
    semcmd.sem_flg = IPC_NOWAIT | SEM_UNDO; // Non-blocking operation, undo on process termination

    // Attempt the semaphore operation (try-wait)
    if (semop(semid, &semcmd, 1) < 0) {
        perror("semop"); // Print error if operation cannot be performed immediately
        return -1;
    }

    return 0; // Return success
}

// Function to perform a signal (V operation) on the semaphore
int semPost(int semid)
{
    struct sembuf semcmd; // Semaphore operation structure

    semcmd.sem_num = 0;       // Semaphore index
    semcmd.sem_op = 1;        // Increment semaphore value (V operation)
    semcmd.sem_flg = SEM_UNDO; // Ensure semaphore is undone if process terminates

    // Perform the semaphore operation (signal)
    if (semop(semid, &semcmd, 1) < 0) {
        perror("semop"); // Print error if operation fails
        return -1;
    }

    return 0; // Return success
}

// Function to get the current value of the semaphore
int semGetValue(int semid)
{
    union semun {
        int val; // Dummy variable for GETVAL operation
    } dummy;

    // Retrieve the current value of the semaphore
    return semctl(semid, 0, GETVAL, dummy);
}

// Function to destroy (remove) a semaphore set
int semDestroy(int semid)
{
    union semun {
        int val; // Dummy variable for IPC_RMID operation
    } dummy;

    // Remove the semaphore set
    if (semctl(semid, 0, IPC_RMID, dummy) < 0) {
        perror("semctl"); // Print error if removal fails
        return -1;
    }

    close(semid); // Close the semaphore ID
    return 0; // Return success
}
