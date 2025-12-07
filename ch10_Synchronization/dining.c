#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_MEN 5 // Number of philosophers (men) at the table
#define NLOOPS 5  // Number of times each philosopher eats and thinks

sem_t ChopStick[NUM_MEN]; // Array of semaphores representing chopsticks

// Function to simulate a timed delay (sleep) for a thread
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;     // Condition variable for timing
    pthread_mutex_t mutex;   // Mutex for synchronizing access
    struct timespec ts;      // Time specification for timed wait
    struct timeval tv;       // To retrieve the current time

    if (pthread_cond_init(&cond, NULL) < 0) { // Initialize condition variable
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&mutex, NULL) < 0) { // Initialize mutex
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    // Get the current time and calculate the wake-up time
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + usecs / 1000000;
    ts.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    // Lock mutex and wait for the specified time
    if (pthread_mutex_lock(&mutex) < 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0) {
        perror("pthread_cond_timedwait");
        pthread_exit(NULL);
    }

    // Clean up condition variable and mutex
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

// Function representing the 'thinking' state of a philosopher
void Thinking(int id)
{
    printf("Philosopher%d: Thinking.....\n", id);
    ThreadUsleep((rand() % 200) * 10000); // Simulate thinking time
    printf("Philosopher%d: Want to eat.....\n", id);
}

// Function representing the 'eating' state of a philosopher
void Eating(int id)
{
    printf("Philosopher%d: Eating.....\n", id);
    ThreadUsleep((rand() % 100) * 10000); // Simulate eating time
}

// Function executed by each philosopher thread
void DiningPhilosopher(int *pId)
{
    int i;
    int id = *pId; // Get the philosopher's ID

    for (i = 0; i < NLOOPS; i++) {
        Thinking(id); // Philosopher starts by thinking

        // Pick up the left and right chopsticks
        if (sem_wait(&ChopStick[id]) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        if (sem_wait(&ChopStick[(id + 1) % NUM_MEN]) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        Eating(id); // Philosopher eats

        // Put down the left and right chopsticks
        if (sem_post(&ChopStick[id]) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
        if (sem_post(&ChopStick[(id + 1) % NUM_MEN]) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
    }

    // Print completion message
    printf("Philosopher%d: thinking & eating %d times.....\n", id, i);
    pthread_exit(NULL);
}

main()
{
    pthread_t tid[NUM_MEN]; // Array to hold thread IDs for philosophers
    int i, id[NUM_MEN];     // IDs for philosophers

    srand(0x8888); // Seed the random number generator

    // Initialize semaphores for each chopstick
    for (i = 0; i < NUM_MEN; i++) {
        if (sem_init(&ChopStick[i], 0, 1) < 0) {
            perror("sem_init");
            exit(1);
        }
        id[i] = i; // Assign IDs to philosophers
    }

    // Create threads for each philosopher
    for (i = 0; i < NUM_MEN; i++) {
        if (pthread_create(&tid[i], NULL, (void *)DiningPhilosopher, (void *)&id[i]) < 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // Wait for all philosopher threads to complete
    for (i = 0; i < NUM_MEN; i++) {
        if (pthread_join(tid[i], NULL) < 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    // Destroy semaphores for chopsticks
    for (i = 0; i < NUM_MEN; i++) {
        if (sem_destroy(&ChopStick[i]) < 0) {
            perror("sem_destroy");
        }
    }
}
