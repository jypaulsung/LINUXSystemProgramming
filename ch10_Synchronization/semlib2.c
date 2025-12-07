#include <stdio.h>
#include <pthread.h>
#include "semlib2.h" // Include the custom semaphore library header

// Function to initialize a semaphore
int sem_init(sem_t *sem, int pshared, int value)
{
    // Check if the semaphore is shared between processes (not supported here)
    if (pshared) {
        fprintf(stderr, "Function not supported\n");
        return -1;
    }

    // Initialize the mutex for the semaphore
    if (pthread_mutex_init(&sem->mutex, NULL) < 0)
        return -1;

    // Initialize the condition variable for the semaphore
    if (pthread_cond_init(&sem->cond, NULL) < 0)
        return -1;

    sem->sval = value; // Set the initial value of the semaphore

    return 0; // Return success
}

// Function to perform a wait (P operation) on the semaphore
int sem_wait(sem_t *sem)
{
    // Lock the mutex to access the semaphore value safely
    if (pthread_mutex_lock(&sem->mutex) < 0)
        return -1;

    // If the semaphore value is zero, wait until it becomes positive
    if (sem->sval == 0) {
        while (sem->sval == 0) {
            // Wait on the condition variable and release the mutex temporarily
            if (pthread_cond_wait(&sem->cond, &sem->mutex) < 0) {
                // Unlock the mutex and return error if condition wait fails
                if (pthread_mutex_unlock(&sem->mutex) < 0)
                    return -1;
                return -1;
            }
        }
        sem->sval--; // Decrement the semaphore value after waiting
    }
    else {
        sem->sval--; // Directly decrement the semaphore value if it's positive
    }

    // Unlock the mutex after updating the semaphore value
    if (pthread_mutex_unlock(&sem->mutex) < 0)
        return -1;

    return 0; // Return success
}

// Function to perform a try-wait (non-blocking P operation) on the semaphore
int sem_trywait(sem_t *sem)
{
    // Lock the mutex to access the semaphore value safely
    if (pthread_mutex_lock(&sem->mutex) < 0)
        return -1;

    // Check if the semaphore value is zero, return immediately if it is
    if (sem->sval == 0) {
        if (pthread_mutex_unlock(&sem->mutex) < 0)
            return -1;
        return -1; // Return failure because semaphore is already 0
    }
    else {
        sem->sval--; // Decrement the semaphore value if it's positive
    }

    // Unlock the mutex after updating the semaphore value
    if (pthread_mutex_unlock(&sem->mutex) < 0)
        return -1;

    return 0; // Return success
}

// Function to perform a signal (V operation) on the semaphore
int sem_post(sem_t *sem)
{
    // Lock the mutex to access the semaphore value safely
    if (pthread_mutex_lock(&sem->mutex) < 0)
        return -1;

    // If the semaphore value is zero, signal one waiting thread
    if (sem->sval == 0) {
        if (pthread_cond_signal(&sem->cond) < 0) {
            if (pthread_mutex_unlock(&sem->mutex) < 0)
                return -1;
            return -1; // Return error if condition signaling fails
        }
    }

    sem->sval++; // Increment the semaphore value

    // Unlock the mutex after updating the semaphore value
    if (pthread_mutex_unlock(&sem->mutex) < 0)
        return -1;

    return 0; // Return success
}

// Function to get the current value of the semaphore
int sem_getvalue(sem_t *sem, int *sval)
{
    *sval = sem->sval; // Set the provided variable to the current semaphore value

    return 0; // Return success
}

// Function to destroy a semaphore
int sem_destroy(sem_t *sem)
{
    // Destroy the mutex associated with the semaphore
    if (pthread_mutex_destroy(&sem->mutex) < 0)
        return -1;

    // Destroy the condition variable associated with the semaphore
    if (pthread_cond_destroy(&sem->cond) < 0)
        return -1;

    return 0; // Return success
}
