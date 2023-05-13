#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define max_numbers_read 100

sem_t full_sem, empty_sem, mutex;
int* buffer;
int buffer_index = 0;
int numbers_read = 0;
int N = 0, NP = 0, NC = 0;
double buffer_occupation;

bool is_prime(double number) {
    if (number < 2)
        return false;

    for (int i = 2; i * i <= number; ++i) {
        if ((int)number % i == 0)
            return false;
    }
    return true;
}

void *producer(void *arg) {
    int producer_id = *(int *)arg;
    free(arg);

    int number;

    while (true) {
        number = rand() % 10000000;
        sem_wait(&empty_sem);
        sem_wait(&mutex);

        if(numbers_read == max_numbers_read) {
                sem_post(&mutex);
                sem_post(&empty_sem);
                sem_post(&full_sem);
                break;
        }

        buffer[buffer_index] = number;
        printf("Produtor %d: Inseriu número %d na posição %d\n", producer_id, number, buffer_index);
        buffer_index = (buffer_index + 1) % N;
        // buffer_occupation = buffer_index / (float)N;
        // printf("%.2f, %d, %d\n", buffer_occupation, buffer_index,N);

        sem_post(&mutex);
        sem_post(&full_sem);

       
    }
     printf("Produtor %d encerrando\n.", producer_id);
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int consumer_id = *(int *)arg;
    free(arg);

    int number;

    while (true) {
        sem_wait(&full_sem);
        sem_wait(&mutex);

        if(numbers_read == max_numbers_read) {
                sem_post(&mutex);
                sem_post(&empty_sem);
                sem_post(&full_sem);
                break;
        }

        number = buffer[(buffer_index - 1 + N) % N];
        numbers_read++;
        printf("Consumidor %d: Leu número %d da posição %d, total de %d números lidos\n", consumer_id,number, buffer_index, numbers_read);
        buffer_index = (buffer_index - 1 + N) % N;


        // buffer_occupation = buffer_index / (float)N;
        // printf("%.2f, %d, %d\n", buffer_occupation, buffer_index, N);
        sem_post(&mutex);
        sem_post(&empty_sem);


        // if (is_prime(number))
        //     printf("Consumidor %d: %d é primo.\n", consumer_id, number);
        // else
        //     printf("Consumidor %d: %d não é primo\n", consumer_id, number);
    }

    printf("Consumidor %d encerrando\n.", consumer_id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s N NP NC\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);  // Tamanho do buffer
    NP = atoi(argv[2]);  // Número de thrads produtoras
    NC = atoi(argv[3]);  // Número de threads consumidoras

    buffer = (int *) malloc(N * sizeof(int));

    sem_init(&full_sem, 0, 0);
    sem_init(&empty_sem, 0, N);
    sem_init(&mutex, 0, 1);

    pthread_t producer_threads[NP];
    pthread_t consumer_threads[NC];

    for (int i = 0; i < NP; ++i) {
        int *producer_id = malloc(sizeof(int));
        *producer_id = i;
        pthread_create(&producer_threads[i], NULL, producer, producer_id);
    }

    for (int i = 0; i < NC; ++i) {
        int *consumer_id = malloc(sizeof(int));
        *consumer_id = i;
        pthread_create(&consumer_threads[i], NULL, consumer, consumer_id);
    }

    for (int i = 0; i < NP; ++i) {
        pthread_join(producer_threads[i], NULL);
    }

    for (int i = 0; i < NC; ++i) {
        pthread_join(consumer_threads[i], NULL);
    }

    printf("saindo\n");
    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
    sem_destroy(&mutex);
    free(buffer);

    return 0;
}