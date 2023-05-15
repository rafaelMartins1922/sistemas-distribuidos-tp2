#!/bin/bash

# Função para calcular a média de uma lista de números
average() {
    sum=0
    count=0
    for num in "$@"; do
        sum=$(echo "$sum + $num" | bc -l)
        count=$((count + 1))
    done
    echo "scale=5; $sum / $count" | bc -l
}

# Verifica se o arquivo CSV existe e cria se não existir
csv_file="avg_time_results.csv"
if [ ! -e "$csv_file" ]; then
    echo "N,K,Tempo (ms)" > "$csv_file"
fi

# Valores de N e K
N=(10000000 100000000 1000000000)
K=(1 2 4 8 16 32 64 128 256)

# Loop para cada combinação de N e K
for n in "${N[@]}"; do
    for k in "${K[@]}"; do
        echo "Executando para N=$n, K=$k"

        # Array para armazenar os tempos de execução
        times=()

        # Executa 10 vezes para cada combinação (N, K)
        for ((i=1; i<=10; i++)); do
            echo "  Execução $i"
            
            # Executa o código C
            output=$(./somador $n $k)
            
            # Extrai o tempo de execução a partir da saída
            time=$(echo "$output" | awk '/Tempo de execução:/ {print $4}')
            times+=("$time")
        done

        # Calcula a média dos tempos de execução
        avg_time=$(average "${times[@]}")

        # Imprime a média
        echo "Média de tempo de execução: $avg_time ms"

        # Armazena os valores no arquivo CSV
        echo "$n,$k,$avg_time" >> "$csv_file"

        echo "========================================"
    done
done

echo "Concluído!"
