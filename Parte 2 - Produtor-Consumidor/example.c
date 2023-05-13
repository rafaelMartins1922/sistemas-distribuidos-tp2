#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void *arg) {
    char *message = (char *)arg;
    int count = 0;

    while (count < 5) {
        printf("%s\n", message);
        count++;
        sleep(1);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t thread1, thread2;
    char *message1 = "Thread 1 is running";
    char *message2 = "Thread 2 is running";

    // Create thread 1
    if (pthread_create(&thread1, NULL, thread_function, (void *)message1) != 0) {
        fprintf(stderr, "Failed to create thread 1\n");
        return 1;
    }

    // Create thread 2
    if (pthread_create(&thread2, NULL, thread_function, (void *)message2) != 0) {
        fprintf(stderr, "Failed to create thread 2\n");
        return 1;
    }

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}



