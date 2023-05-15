#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define max_numbers_read 100000

sem_t full_sem, empty_sem, mutex;
int* buffer;
int buffer_index = 0;
int numbers_read = 0;
int N = 0, NP = 0, NC = 0;
static pthread_barrier_t bar;

int occupation_index = 0;
double occupation_over_time[2][300000], buffer_occupation = 0;

struct timeval start, end;
double execution_time; 

char* times_filename;
char* occupation_filename;

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
    pthread_barrier_wait(&bar);
    int producer_id = *(int *)arg;
    free(arg);

    int number;

    while (true) {
        number = rand() % 10000001;
        sem_wait(&empty_sem);
        sem_wait(&mutex);

        if(numbers_read == max_numbers_read) {
                sem_post(&mutex);
                sem_post(&empty_sem);
                sem_post(&full_sem);
                break;
        }

        buffer[buffer_index] = number;
        buffer_occupation++;

        gettimeofday(&end, NULL);
        
        occupation_over_time[0][occupation_index] = (buffer_occupation) / (float) N;
        occupation_over_time[1][occupation_index] = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        occupation_index++;
        buffer_index = (buffer_index + 1) % N;

        sem_post(&mutex);
        sem_post(&full_sem);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    pthread_barrier_wait(&bar);
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
        buffer_index = (buffer_index - 1 + N) % N;
        buffer_occupation--;

        gettimeofday(&end, NULL);
        occupation_over_time[0][occupation_index] = (buffer_occupation) / (float) N;
        occupation_over_time[1][occupation_index] = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        occupation_index++;

        sem_post(&mutex);
        sem_post(&empty_sem);


        if (is_prime(number))
            printf("Consumidor %d: %d é primo.\n", consumer_id, number);
        else
            printf("Consumidor %d: %d não é primo\n", consumer_id, number);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    gettimeofday(&start, NULL);

    if (argc != 6) {
        printf("Uso: %s <N> <NP> <NC> <nome_arquivo_tempos> <nome_arquivo_ocupacao>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);  // Tamanho do buffer
    NP = atoi(argv[2]);  // Número de thrads produtoras
    NC = atoi(argv[3]);  // Número de threads consumidoras
    times_filename = argv[4]; //Nome do arquivo onde o tempo de execução será registrado
    occupation_filename = argv[5];// Nome do arquivo onde a ocupação do buffer será registrada

    buffer = (int *) malloc(N * sizeof(int));

    sem_init(&full_sem, 0, 0);
    sem_init(&empty_sem, 0, N);
    sem_init(&mutex, 0, 1);

    pthread_t producer_threads[NP];
    pthread_t consumer_threads[NC];
     pthread_barrier_init(&bar, NULL, NP+NC);

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

    if(strcmp(times_filename, "") != 0) {
         gettimeofday(&end, NULL); 

        FILE *times_file = fopen(times_filename, "a+");
        if (times_file == NULL) {
            printf("Falha ao abrir o arquivo CSV.\n");
            return 1;
        }

        fprintf(times_file, "%d, %d, %d, %f\n", N, NP, NC, (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0);
        fclose(times_file);
    }
   
    
    if(strcmp(occupation_filename, "") != 0) {
        FILE *occupation_file = fopen(occupation_filename, "a+");
        if (occupation_file == NULL) {
            printf("Falha ao abrir o arquivo CSV.\n");
            return 1;
        }

        for(int i = 0; i < occupation_index; ++i) {
            fprintf(occupation_file, "%.3f, %.5f\n", occupation_over_time[0][i], occupation_over_time[1][i]);
        }
        fclose(occupation_file);
    }

    // Destruição dos semáforos e barreiras e liberação das variáveis de memória dinâmica
    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
    sem_destroy(&mutex);
    free(buffer);
    pthread_barrier_destroy(&bar);
    return 0;
}