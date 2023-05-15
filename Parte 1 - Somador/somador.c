#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define NUMBER_VECTOR_LENGTH 10000000 // tamanho do vetor de números

// Struct para organizar os argumentos usados pelas threads
typedef struct {
    pthread_t thread_id;
    int thread_index;
    int num_threads;
    int n_overloaded_threads;
    int8_t* numbers;
    long int chunk_size;
    atomic_flag* lock;
    long long int* sum;
} ThreadArgs; 

// Spinlock com busy wait
void acquire(atomic_flag* lock) {
    while (atomic_flag_test_and_set(lock));
}

void release(atomic_flag* lock) {
    atomic_flag_clear(lock);
}

// Soma paralela executadas pelas threads
void* thread_sum(void* arg) {
    ThreadArgs* args = (ThreadArgs*) arg;
    long int start, end;
    long long int partial_sum = 0;

    // Organizando os indices 
    if (args->thread_index < args->n_overloaded_threads)
        start = args->thread_index * args->chunk_size;
    else    
        start = args->thread_index * args->chunk_size + args->n_overloaded_threads;
    end = start + args->chunk_size;

    // Soma do conjunto da thread
    for (long int i = start; i < end; i++) {
        partial_sum += args->numbers[i];
    }

    acquire(args->lock);
    *(args->sum) += partial_sum;
    release(args->lock);

    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int status, n_threads, n_overloaded_threads;
    long int chunk_size; 
    long long int sum, correct_sum;
    int8_t* numbers;
    atomic_flag lock = ATOMIC_FLAG_INIT;
    ThreadArgs* thread_args;
    double elapsed_time = 0;
    clock_t start_time, end_time;

    if (argc != 2) {
        printf("Uso: %s <n_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    n_threads = atoi(argv[1]);
    chunk_size = floor(NUMBER_VECTOR_LENGTH / n_threads);
    n_overloaded_threads = NUMBER_VECTOR_LENGTH % n_threads;

    // Repete a execução por 10 vezes para obter um tempo mais preciso
    for (int exec_iter = 0; exec_iter < 10; exec_iter++) {

        // Aloca o vetor de números aleatórios
        numbers = (int8_t*) malloc(NUMBER_VECTOR_LENGTH * sizeof(int8_t));
        if (numbers == NULL) {
            printf("Erro: Não foi possível alocar memória para os números.\n");
            exit(EXIT_FAILURE);
        }

        // Preenche o vetor com números aleatórios entre -100 e 100
        srand(clock()); // Usado clock como seed devido ao tempo curto das iterações em alguns casos
        for (long int i = 0; i < NUMBER_VECTOR_LENGTH; i++) {
            numbers[i] = rand() % 201 - 100;
        }

        // Aloca o array dos argumentos das threads
        thread_args = (ThreadArgs*) malloc(n_threads * sizeof(ThreadArgs));
        if (thread_args == NULL) {
            printf("Erro: Não foi possível alocar memória para argumentos das threads.\n");
            exit(EXIT_FAILURE);
        }

        // Inicializa a estrutura de argumentos das threads sem o ID
        for (int i = 0; i < n_threads; i++) {
            thread_args[i].thread_index = i;
            thread_args[i].num_threads = n_threads;
            thread_args[i].n_overloaded_threads = n_overloaded_threads;
            thread_args[i].numbers = numbers;
            thread_args[i].lock = &lock;
            thread_args[i].sum = &sum;

            if (i < n_overloaded_threads) {
                thread_args[i].chunk_size = chunk_size + 1;
            }
            else {
                thread_args[i].chunk_size = chunk_size;
            }
        }

        // Cria as threads e calcula a soma utilizando elas
        start_time = clock();
        sum = 0;
        for (int i = 0; i < n_threads; i++) {
            status = pthread_create(&thread_args[i].thread_id, NULL, &thread_sum, &thread_args[i]);
            if (status != 0) {
                printf("Erro: Não foi possível criar threads.");
                exit(EXIT_FAILURE);
            }
        }
        for (int i = 0; i < n_threads; i++) {
            status = pthread_join(thread_args[i].thread_id, NULL);
            if (status != 0) {
                printf("Erro: Não foi possível aguardar a finalização das threads.");
                exit(EXIT_FAILURE);
            }
        }
        end_time = clock();

        // Calcula a soma correta e verifica o resultado
        correct_sum = 0;
        printf("Soma %d: \n", exec_iter+1);
        for (long int i = 0; i < NUMBER_VECTOR_LENGTH; i++) {
            correct_sum += numbers[i];
        }
        if (sum != correct_sum) {
            printf("Erro: a soma está incorreta (esperado %lld, obtido %lld).\n",
                    correct_sum, sum);
        } else {
            printf("Sucesso.\nResultado: %lld\n\n", sum);
        }

        elapsed_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;

        free(numbers);
        free(thread_args);
    }

    elapsed_time = (elapsed_time / 10) * 1e3;
    printf("Tempo médio de execução: %.5f ms\n", elapsed_time);

    exit(EXIT_SUCCESS);
}
