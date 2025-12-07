#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_MEN 5 // Number of philosophers
#define NLOOPS 5  // Number of times each philosopher thinks and eats

#define LEFT(i) ((i + NUM_MEN - 1) % NUM_MEN) // Get left neighbor
#define RIGHT(i) ((i + 1) % NUM_MEN)          // Get right neighbor

enum { THINKING, EATING, HUNGRY }; // States for philosophers

sem_t Philosopher[NUM_MEN]; // Semaphores for each philosopher
sem_t Mutex;                // Mutex to control critical section
int State[NUM_MEN];         // Array to track each philosopher's state

// Function to simulate a timed delay for a thread
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    struct timespec ts;
    struct timeval tv;

    if (pthread_cond_init(&cond, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + usecs / 1000000;
    ts.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    if (pthread_mutex_lock(&mutex) < 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0) {
        perror("pthread_cond_timedwait");
        pthread_exit(NULL);
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

// Simulate the philosopher's thinking state
void Thinking(int id)
{
    printf("Philosopher%d: Thinking.....\n", id);
    ThreadUsleep((rand() % 200) * 10000);
    printf("Philosopher%d: Want to eat.....\n", id);
}

// Simulate the philosopher's eating state
void Eating(int id)
{
    printf("Philosopher%d: Eating.....\n", id);
    ThreadUsleep((rand() % 100) * 10000);
}

// Test if a philosopher can start eating
void Test(int id)
{
    if (State[id] == HUNGRY && State[LEFT(id)] != EATING && State[RIGHT(id)] != EATING) {
        State[id] = EATING;
        sem_post(&Philosopher[id]); // Signal the philosopher to eat
    }
}

// Pick up chopsticks (enter the eating state)
void PickUp(int id)
{
    sem_wait(&Mutex);          // Enter critical section
    State[id] = HUNGRY;        // Mark the philosopher as hungry
    Test(id);                  // Check if the philosopher can eat
    sem_post(&Mutex);          // Exit critical section
    sem_wait(&Philosopher[id]); // Wait until allowed to eat
}

// Put down chopsticks (leave the eating state)
void PutDown(int id)
{
    sem_wait(&Mutex);           // Enter critical section
    State[id] = THINKING;       // Mark the philosopher as thinking
    Test(LEFT(id));             // Check if the left neighbor can eat
    Test(RIGHT(id));            // Check if the right neighbor can eat
    sem_post(&Mutex);           // Exit critical section
}

// Function executed by each philosopher thread
void DiningPhilosopher(int *pId)
{
    int i;
    int id = *pId;

    for (i = 0; i < NLOOPS; i++) {
        Thinking(id);  // Simulate thinking state
        PickUp(id);    // Try to pick up chopsticks
        Eating(id);    // Simulate eating state
        PutDown(id);   // Put down chopsticks
    }

    printf("Philosopher%d: thinking & eating %d times.....\n", id, i);
    pthread_exit(NULL);
}

main()
{
    pthread_t tid[NUM_MEN]; // Array to hold philosopher threads
    int i, id[NUM_MEN];     // IDs for philosophers

    srand(0x8888); // Seed for random delays

    // Initialize semaphores
    for (i = 0; i < NUM_MEN; i++) {
        if (sem_init(&Philosopher[i], 0, 0) < 0) {
            perror("sem_init");
            exit(1);
        }
        id[i] = i; // Assign IDs
    }
    if (sem_init(&Mutex, 0, 1) < 0) {
        perror("sem_init");
        exit(1);
    }

    // Create philosopher threads
    for (i = 0; i < NUM_MEN; i++) {
        if (pthread_create(&tid[i], NULL, (void *)DiningPhilosopher, (void *)&id[i]) < 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // Wait for all threads to finish
    for (i = 0; i < NUM_MEN; i++) {
        if (pthread_join(tid[i], NULL) < 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    // Destroy semaphores
    for (i = 0; i < NUM_MEN; i++) {
        if (sem_destroy(&Philosopher[i]) < 0) {
            perror("sem_destroy");
        }
    }
    sem_destroy(&Mutex);
}
