#!/bin/bash

start_time=$(date +%s.%N)  # Captura o horário de início em segundos com nanossegundos

# Executa o programa
gcc -pthread -o produtor_consumidor produtor_consumidor.c
./produtor_consumidor 10 10 10

end_time=$(date +%s.%N)  # Captura o horário de término em segundos com nanossegundos

execution_time=$(echo "$end_time - $start_time" | bc)  # Calcula o tempo de execução

echo "Tempo de execução: $execution_time segundos"