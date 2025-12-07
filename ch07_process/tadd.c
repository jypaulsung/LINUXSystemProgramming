#include <stdio.h>
#include <pthread.h>

// data structure to pass arguments to the thread function
typedef struct {
    int start;
    int end;
    int sum;
} ThreadData;

// thread function to calculate the sum of numbers from start to end
void* calculate_sum(void* arg) { // take a pointer to a ThreadData structure as an argument
    ThreadData* data = (ThreadData*)arg;
    data->sum = 0;
    for (int i = data->start; i <= data->end; ++i) {
        data->sum += i;
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    ThreadData data1 = {1, 50, 0};
    ThreadData data2 = {51, 100, 0};

    // create two threads to calculate the sum of numbers from 1 to 50 and from 51 to 100
    pthread_create(&thread1, NULL, calculate_sum, &data1); // NULL : use the default thread attributes
    pthread_create(&thread2, NULL, calculate_sum, &data2);

    // wait for the threads to terminate
    // the main thread will wait until both threads have finished executing
    pthread_join(thread1, NULL); // NULL : for the location where the exit status of the joined thread will be stored
    pthread_join(thread2, NULL); // NULL : we are not interested in the exit status of the joined thread

    int total_sum = data1.sum + data2.sum;
    printf("Total sum from 1 to 100 is: %d\n", total_sum);

    return 0;
}
